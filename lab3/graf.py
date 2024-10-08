'''
Esse código está horripilante, não reproduza-o!
'''

import pandas as pd
import matplotlib.pyplot as plt
import sys,os

if(len(sys.argv) < 3):
    print(f"erro: uso {sys.argv[0]} <arq-csv> <pasta-para-graficos>",file=sys.stderr)
    exit(1)

if(not os.path.exists(sys.argv[2])):
    os.makedirs(sys.argv[2])
    print(f"foi criada a pasta {sys.argv[2]}")

df = pd.read_csv(sys.argv[1])

means = df.groupby(['Dimensao','NumThreads'])['TempoTotal'].mean().reset_index()
dimensions = means['Dimensao'].unique()
num_threads = means['NumThreads'].unique()

print(dimensions)
for d in dimensions:
    data = means[means['Dimensao'] == d].copy()
    plt.figure(figsize=(10,6))
    plt.plot(data['NumThreads'],data['TempoTotal'], marker='o',linestyle='-', label=f"Dimensao:{d}")
    plt.xticks(num_threads)
    plt.yticks(data['TempoTotal'])
    plt.title(f'Tempo Total por Num Threads (Dimensao: {d})')
    plt.xlabel('Num Threads')
    plt.ylabel('Tempo Total')
    plt.grid(True)
    plt.legend()
    plt.savefig(f'{sys.argv[2]}/tempo_total_dimensao_{d}.png',format='png')
    plt.close()

#grafico de aceleracao
plt.figure(figsize=(10,6))
plt.title(f'Aceleração por Num Threads')
plt.xticks(data['NumThreads'])
Y_acc = []
for i, d in enumerate(dimensions):
    data = means[means['Dimensao'] == d].copy()
    tseq = data.iloc[0]['TempoTotal']
    print(tseq)
    data['acceleration'] = tseq/data['TempoTotal']
    if i%2 : Y_acc.extend(data['acceleration'])
    plt.plot(data['NumThreads'],data['acceleration'], marker='o',linestyle='-', label=f"Dimensao:{d}")
plt.yticks(Y_acc)
plt.xlabel('Num Threads')
plt.ylabel('Aceleração')
plt.grid(True)
plt.legend()
plt.savefig(f'{sys.argv[2]}/aceleracao_dimensao.png',format='png')

#grafico de eficiencia
plt.figure(figsize=(10,6))
plt.title(f'Eficiência por Num Threads')
plt.xticks(data['NumThreads'])
Y_eff = []
for i,d in enumerate(dimensions):    
    data = means[means['Dimensao'] == d].copy()
    tseq = data.iloc[0]['TempoTotal']
    print(tseq)
    data['acceleration'] = tseq/data['TempoTotal']
    data['efficiency'] = data['acceleration']/data['NumThreads']
    if i%2: Y_eff.extend(data['efficiency'])
    plt.plot(data['NumThreads'],data['efficiency'], marker='o',linestyle='-', label=f"Dimensao:{d}")
plt.yticks(Y_eff)
plt.xlabel('Num Threads')
plt.ylabel('Eficiências')
plt.grid(True)
plt.legend()
plt.savefig(f'{sys.argv[2]}/eficiencia_dimensao.png',format='png')

''' Nota para vida depois dos resultados, sempre... SEMPRE usar -O3'''