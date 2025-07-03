import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

def parse_file(path, name):
    data = []
    current_test = {}  # Armazenará os dados completos de um teste (BB + Aprox)
    last_method = None

    with open(path, 'r', encoding='utf-8') as f:
        for line in f:
            line = line.strip()

            # Dados do problema
            if line.startswith('* Número de itens: '):
                # Se já temos dados do método anterior, prepara para novo teste
                if last_method == '2-aproximativo-guloso':
                    data.append(current_test)
                    current_test = {}
                current_test['N'] = line.split(': ')[1].strip()
            elif line.startswith('* Peso médio:'):
                current_test['w_avg'] = line.split(': ')[1].strip()
            elif line.startswith('* Peso máximo:'):
                current_test['w_max'] = line.split(': ')[1].strip()
            elif line.startswith('* Valor médio:'):
                current_test['v_avg'] = line.split(': ')[1].strip()
            elif line.startswith('* Valor máximo:'):
                current_test['v_max'] = line.split(': ')[1].strip()
            elif line.startswith('* Capacidade da Mochila:'):
                current_test['W'] = line.split(': ')[1].strip()
            elif line.startswith('* Solução ótima: '):
                current_test['sol'] = line.split(': ')[1].strip()

            # Branch and Bound
            elif line.startswith('Branch and Bound:'):
                current_test['BB_sol'] = line.split(': ')[1].strip()
                last_method = 'BB'
            elif line.startswith('Tempo Branch and Bound :'):
                current_test['BB_time'] = line.split(': ')[1].strip()
            elif line.startswith('* Memória residente real (RSS):') and '[BB]' in current_test.get('last_section', ''):
                current_test['BB_mem'] = line.split(': ')[1].replace(' KB', '').strip()

            # FPTAS
            elif line.startswith('2-aproximativo-fptas:'):
                current_test['FPTAS_sol'] = line.split(': ')[1].strip()
                last_method = '2-aproximativo-fptas'
            elif line.startswith('Tempo Aproximativo FPTAS:'):
                current_test['FPTAS_time'] = line.split(': ')[1].strip()
            elif line.startswith('* Memória residente real (RSS):') and '[fptas]' in current_test.get('last_section', ''):
                current_test['FPTAS_mem'] = line.split(': ')[1].replace(' KB', '').strip()
                current_test['type'] = name

             # Guloso
            elif line.startswith('2-aproximativo-guloso:'):
                current_test['greedy_sol'] = line.split(': ')[1].strip()
                last_method = '2-aproximativo-guloso'
            elif line.startswith('Tempo Aproximativo Guloso:'):
                current_test['greedy_time'] = line.split(': ')[1].strip()
            elif line.startswith('* Memória residente real (RSS):') and '[guloso]' in current_test.get('last_section', ''):
                current_test['greedy_mem'] = line.split(': ')[1].replace(' KB', '').strip()
                current_test['type'] = name

            # Verificando a sessão atual
            elif line.startswith('=== Métricas de Memória'):
                current_test['last_section'] = line

    # Adicionando último teste se existir
    if current_test:
        data.append(current_test)

    return pd.DataFrame(data)

# Conversor de notação
def clean_value(val):
    if not isinstance(val, str):
        return val
    if 'e+' in val:
        return str(float(val))
    return val.replace('KB', '')

# Processando ambos os arquivos
df_large = parse_file("results/large_results.txt", "Large")
df_low = parse_file("results/low_results.txt", "Low")

# Combinando
df_combined = pd.concat([df_large, df_low], ignore_index=True)

df_combined = df_combined.map(clean_value)

for col in [ 'N', 'W','sol','w_avg','w_max','v_avg','v_max','BB_sol', 'BB_time', 'BB_mem', 
           'FPTAS_sol', 'FPTAS_time', 'FPTAS_mem','greedy_sol', 'greedy_time', 'greedy_mem']:
    df_combined[col] = pd.to_numeric(df_combined[col], errors='coerce')

# Reordenando
columns = ['type', 'N', 'W','sol','w_avg','w_max','v_avg','v_max','BB_sol', 'BB_time', 'BB_mem', 
           'FPTAS_sol', 'FPTAS_time', 'FPTAS_mem','greedy_sol', 'greedy_time', 'greedy_mem']
df_combined = df_combined[columns]

df_low = df_combined[df_combined['type'] == 'Low'].sort_values(by=['N','W'],ignore_index=True).copy()
df_high = df_combined[df_combined['type'] == 'Large'].sort_values(by=['N','W'],ignore_index=True).copy()

#Tempo
plt.figure(figsize=(12, 6))
plt.title('Tempo de Execução para Problemas de Baixa Complexidade')
plt.yscale('log')
plt.bar(np.arange(len(df_low)) - 0.3, df_low['BB_time'], color='red', width=0.3, label='Tempo Branch and Bound', alpha=1)
plt.bar(np.arange(len(df_low)), df_low['FPTAS_time'], color='green', width=0.3, label='Tempo FPTAS', alpha=1)
plt.bar(np.arange(len(df_low)) + 0.3, df_low['greedy_time'], color='blue', width=0.3, label='Tempo Guloso', alpha=1)
plt.xticks(
    ticks=range(len(df_low)),
    labels=[
        f"N={n_val}\nW={W_val}\nwmax:{w_max}\nwavg:{w_avg}\nvmax:{v_max}\nvavg:{v_avg}"
        for n_val, W_val, w_max, w_avg, v_max, v_avg in zip(
            df_low['N'], df_low['W'], df_low['w_max'], df_low['w_avg'], df_low['v_max'], df_low['v_avg']
        )
    ],
    rotation=0,
    fontsize=8
)
plt.ylabel('Tempo (ms) - Escala Logarítmica')
plt.legend()
plt.savefig(bbox_inches='tight',fname="results/images/tempo_low.png")

plt.figure(figsize=(28, 6))
plt.title("Tempo de Execução para Problemas de Alta Complexidade")

bab_timeout = df_high[df_high['BB_time'].isna()]
fptas_timeout = df_high[df_high['FPTAS_time'].isna()]

plt.yscale('log')
plt.bar(np.arange(len(df_high)) - 0.3, df_high['BB_time'], color='red', width=0.3, label='Tempo Branch and Bound', alpha=1)
plt.bar(bab_timeout.index-0.3,1800000, color='red', width=0.3, alpha=0.1)
plt.bar(np.arange(len(df_high)), df_high['FPTAS_time'], color='green', width=0.3, label='Tempo FPTAS', alpha=1)
plt.bar(fptas_timeout.index,1800000, color='green', width=0.3, alpha=0.1)
plt.bar(np.arange(len(df_high)) + 0.3, df_high['greedy_time'], color='blue', width=0.3, label='Tempo Guloso', alpha=1)
plt.axhline(y=1800000, color='black', linestyle='--', label='Timeout após 30 minutos',alpha=0.3)
plt.xticks(
    ticks=range(len(df_high)),
    labels=[
        f"N={n_val}\nW={W_val}\nwmax:{w_max}\nwavg:{w_avg}\nvmax:{v_max}\nvavg:{v_avg}"
        for n_val, W_val, w_max, w_avg, v_max, v_avg in zip(
            df_high['N'], df_high['W'], df_high['w_max'], df_high['w_avg'], df_high['v_max'], df_high['v_avg']
        )
    ],
    rotation=0,
    fontsize=8
)
plt.ylabel('Tempo (ms) - Escala Logarítmica')
plt.legend()
plt.savefig(bbox_inches='tight',fname="results/images/tempo_high.png")

#Espaço
plt.figure(figsize=(12, 6))
plt.title('Uso de memória para Problemas de Baixa Complexidade')
plt.bar(np.arange(len(df_low)) - 0.3, df_low['BB_mem'], color='red', width=0.3, label='Custo Branch and Bound', alpha=1)
plt.bar(np.arange(len(df_low)), df_low['FPTAS_mem'], color='green', width=0.3, label='Custo FPTAS', alpha=1)
plt.bar(np.arange(len(df_low)) + 0.3, df_low['greedy_mem'], color='blue', width=0.3, label='Custo Guloso', alpha=1)
plt.xticks(
    ticks=range(len(df_low)),
    labels=[
        f"N={n_val}\nW={W_val}\nwmax:{w_max}\nwavg:{w_avg}\nvmax:{v_max}\nvavg:{v_avg}"
        for n_val, W_val, w_max, w_avg, v_max, v_avg in zip(
            df_low['N'], df_low['W'], df_low['w_max'], df_low['w_avg'], df_low['v_max'], df_low['v_avg']
        )
    ],
    rotation=0,
    fontsize=8
)
plt.ylabel('Uso de memória (KB)')
plt.legend(loc='lower left',framealpha=1)
plt.savefig(bbox_inches='tight',fname="results/images/custo_low.png")

plt.figure(figsize=(28, 6))
plt.title("Uso de memória para Problemas de Alta Complexidade")
plt.yscale('log')
plt.bar(np.arange(len(df_high)) - 0.2, df_high['BB_mem'], color='red', width=0.3, label='Custo Branch and Bound', alpha=1)
plt.bar(np.arange(len(df_high)), df_high['FPTAS_mem'], color='green', width=0.3, label='Custo FPTAS', alpha=1)
plt.bar(np.arange(len(df_high)) + 0.3, df_high['greedy_mem'], color='blue', width=0.3, label='Custo Guloso', alpha=1)
plt.xticks(
    ticks=range(len(df_high)),
    labels=[
        f"N={n_val}\nW={W_val}\nwmax:{w_max}\nwavg:{w_avg}\nvmax:{v_max}\nvavg:{v_avg}"
        for n_val, W_val, w_max, w_avg, v_max, v_avg in zip(
            df_high['N'], df_high['W'], df_high['w_max'], df_high['w_avg'], df_high['v_max'], df_high['v_avg']
        )
    ],
    rotation=0,
    fontsize=8
)
plt.ylabel('Uso de memória (KB) - Escala Logarítmica')
plt.legend()
plt.savefig(bbox_inches='tight',fname="results/images/custo_high.png")

#Precisão
plt.figure(figsize=(12, 6))
plt.title("Precisão para Problemas de Baixa Complexidade")
plt.bar(np.arange(len(df_low)) - 0.3, 100 * df_low['BB_sol']/df_low['sol'], color='red', width=0.3, label='Solução Branch and Bound', alpha=1)
plt.bar(np.arange(len(df_low)), 100 *df_low['FPTAS_sol']/df_low['sol'], color='green', width=0.3, label='Solução FPTAS', alpha=1)
plt.bar(np.arange(len(df_low)) + 0.3, 100 *  df_low['greedy_sol']/df_low['sol'], color='blue', width=0.3, label='Solução Guloso', alpha=1)
plt.xticks(
    ticks=range(len(df_low)),
    labels=[
        f"N={n_val}\nW={W_val}\nwmax:{w_max}\nwavg:{w_avg}\nvmax:{v_max}\nvavg:{v_avg}"
        for n_val, W_val, w_max, w_avg, v_max, v_avg in zip(
            df_low['N'], df_low['W'], df_low['w_max'], df_low['w_avg'], df_low['v_max'], df_low['v_avg']
        )
    ],
    rotation=0,
   fontsize=8
)
plt.axhline(y=100, color='orange', linestyle='--', label='100% da solução ótima',alpha=0.5)
plt.axhline(y=50, color='black', linestyle='--', label='50% da solução ótima',alpha=0.3)
plt.ylabel('Resultado em relação à solução ótima (%)')
plt.yticks([0,25,50,75,100])
plt.legend(loc='lower right',framealpha=1)
plt.savefig(bbox_inches='tight',fname="results/images/precisao_low.png")

print(df_high[['BB_sol', 'FPTAS_sol', 'greedy_sol', 'sol']])

plt.figure(figsize=(28, 6))
plt.title("Precisão para Problemas de Alta Complexidade")
plt.bar(np.arange(len(df_high)) - 0.3, 100 * df_high['BB_sol']/df_high['sol'], color='red', width=0.3, label='Solução Branch and Bound', alpha=1)
plt.bar(np.arange(len(df_high)), 100 * df_high['FPTAS_sol']/df_high['sol'], color='green', width=0.3, label='Solução FPTAS', alpha=1)
plt.bar(np.arange(len(df_high)) + 0.3, 100 * df_high['greedy_sol']/df_high['sol'], color='blue', width=0.3, label='Solução Guloso', alpha=1)
plt.xticks(
    ticks=range(len(df_high)),
    labels=[
        f"N={n_val}\nW={W_val}\nwmax:{w_max}\nwavg:{w_avg}\nvmax:{v_max}\nvavg:{v_avg}"
        for n_val, W_val, w_max, w_avg, v_max, v_avg in zip(
            df_high['N'], df_high['W'], df_high['w_max'], df_high['w_avg'], df_high['v_max'], df_high['v_avg']
        )
    ],
    rotation=0,
    fontsize=8
    )
plt.axhline(y=100, color='orange', linestyle='--', label='100% da solução ótima',alpha=0.5)
plt.axhline(y=50, color='black', linestyle='--', label='50% da solução ótima',alpha=0.3)
plt.ylabel('Resultado em relação à solução ótima (%)')
plt.yticks([0,25,50,75,100])
plt.legend(loc='lower left',framealpha=1)
plt.savefig(bbox_inches='tight',fname="results/images/precisao_high.png")

df_combined = df_combined.fillna('')
df_combined.to_csv('results/resultados_combinados.csv', index=False)
