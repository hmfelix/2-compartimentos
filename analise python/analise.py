# scrpit de analise dos resultados da simulacao

import os
import pandas as pd
from matplotlib import pyplot as plt

# caminho dos arquivos
diretorio_mae = os.path.dirname(os.getcwd())
diretorio_dados_brutos = os.path.join(diretorio_mae, "resultados brutos")
caminhos_arquivos_brutos = [os.path.join(diretorio_dados_brutos, i) for i in os.listdir(diretorio_dados_brutos)]
diretorio_medias = os.path.join(diretorio_mae, "resultados consolidados (medias)")
caminhos_arquivos_medias = [os.path.join(diretorio_medias, i) for i in os.listdir(diretorio_medias)]

# importacao dos dados
dados_brutos = [pd.read_csv(i) for i in caminhos_arquivos_brutos]
dados_medias = [pd.read_csv(i) for i in caminhos_arquivos_medias]

# especificacoes das configuracoes:
configs = [
    ": \nN=$10^3$, $C_{norm}^{(A)}$=0.5, $C_{norm}^{(B)}$=0.25",
    ": \nN=$10^3$, $C_{norm}^{(A)}$=0.5, $C_{norm}^{(B)}$=0.005",
    ": \nN=$10^3$, $C_{norm}^{(A)}$=0.05, $C_{norm}^{(B)}$=0.025",
    ": \nN=$10^3$, $C_{norm}^{(A)}$=0.05, $C_{norm}^{(B)}$=0.0005",
    ": \nN=$10^7$, $C_{norm}^{(A)}$=0.5, $C_{norm}^{(B)}$=0.25",
    ": \nN=$10^7$, $C_{norm}^{(A)}$=0.5, $C_{norm}^{(B)}$=0.005",
    ": \nN=$10^7$, $C_{norm}^{(A)}$=0.05, $C_{norm}^{(B)}$=0.025",
    ": \nN=$10^7$, $C_{norm}^{(A)}$=0.05, $C_{norm}^{(B)}$=0.0005"
]

# plots (por enquanto apenas das medias)
fig, axs = plt.subplots(4, 2, figsize=(7,9)) # cria objetos para plotagem de 8 graficos
for i in range(8):
    ax = axs.flat[i] # flat = iterador sobre os subplots, permite selecionar cada um
    t = dados_medias[i]['t']
    C_A = dados_medias[i]["C_A"]
    C_B = dados_medias[i]["C_B"]
    delta = dados_medias[i]["delta"]
    ax.plot(t, C_A, label="$C^{(A)}$")
    ax.plot(t, C_B, label="$C^{(B)}$")
    ax.plot(t, delta, label="$\Delta C$")
    ax.set_title("Configuração " + str(i+1) + configs[i])
    ax.set_xlabel("Passos MC")
    ax.set_ylabel("Concentrações")
    ax.legend(loc='center', ncol=3, handlelength=.7, handletextpad=0.5, columnspacing=.5, bbox_to_anchor=(0.5, 0.28))
fig.tight_layout()

# salvando:
plt.savefig(os.path.join(diretorio_mae, "img/resultados.svg"), format="svg")
plt.close()