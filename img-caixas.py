from matplotlib import pyplot as plt
import random

# cria dois plots quadrados
fig, axs = plt.subplots(nrows=1, ncols=2, figsize=(8, 4))

# remove espaço entre os plots
plt.tight_layout()
plt.subplots_adjust(wspace=0)

# caixa esquerda:
# adicionar grids
axs[0].grid(which="major", linewidth=2)
axs[0].minorticks_on()
axs[0].grid(which="minor", linewidth=2)
# esconder os numeros e ticks dos eixos
axs[0].set_xticklabels([])
axs[0].set_yticklabels([])
axs[0].tick_params(which="both", length=0)

# caixa direita:
# adicionar grids
axs[1].grid(which="major", linewidth=2)
axs[1].minorticks_on()
axs[1].grid(which="minor", linewidth=2)
# esconder os numeros e ticks dos eixos
axs[1].set_xticklabels([])
axs[1].set_yticklabels([])
axs[1].tick_params(which="both", length=0)

# substitui fronteiras automaticas (nao apagaveis)
# por uma fronteira manual (para poder ser sobreposta pela abertura):
axs[0].set_frame_on(False)
axs[1].set_frame_on(False)
axs[0].plot([0.005, 0.995, 0.995, 0.005, 0.005], [0.005, 0.005, 0.997, 0.997, 0], color='black', linewidth=2)
axs[1].plot([0.005, 0.995, 0.995, 0.005, 0.005], [0.005, 0.005, 0.997, 0.997, 0], color='black', linewidth=2)

# pintar regiao da abertura:
axs[0].scatter([0.974],[0.525], marker='s', s=100, color="skyblue")
axs[1].scatter([0.025],[0.525], marker='s', s=100, color="skyblue")

# abertura entre as caixas:
# fixar o grafico na regiao de interesse
axs[0].set_xlim([0, 1])
axs[0].set_ylim([0, 1])
axs[1].set_xlim([0, 1])
axs[1].set_ylim([0, 1])
# sobrepor o eixo dos graficos com um buraco branco
axs[0].plot([1, 1], [0.51, 0.54], color="skyblue", linewidth=6)
axs[1].plot([0, 0], [0.51, 0.54], color="skyblue", linewidth=6)
# sobrepor divisoria pontilhada
axs[0].plot([1, 1], [0.51, 0.54], "--", color="gray", linewidth=1)
axs[1].plot([0, 0], [0.51, 0.54], "--", color="gray", linewidth=1)
# adicionar fronteira interna da arbetura
axs[0].plot([0.98, 1], [0.55, 0.55], color="black", linewidth=3)
axs[0].plot([0.98, 1], [0.5, 0.5], color="black", linewidth=3)
axs[1].plot([0, 0.026], [0.55, 0.55], color="black", linewidth=3)
axs[1].plot([0, 0.026], [0.5, 0.5], color="black", linewidth=3)

# preencher as caixas com particulas:
# sao 400 locais em cada caixa
# usando o exemplo de c/c0 = 0.2 na primeira caixa e 0.05 na segunda,
# temos 40 particulas na primeira e 10 na segunda

# gerando numeros aleatorios para as localizacoes:
# 40 numeros para a primeira caixa
particulas_caixa_A = random.sample(range(0,399), 40)
ds = 0.05 # comprimento de cada celula
offset = 0.025 # salto inicial para nao cair nas bordas
linhas_caixa_A = [offset + int(x/20)*ds for x in particulas_caixa_A] # coordenada da linha de cada numero
colunas_caixa_A = [offset + x%20*ds for x in particulas_caixa_A] # coordenada de coluna
axs[0].scatter(linhas_caixa_A, colunas_caixa_A) # adicionando as particulas ao grafico
# 10 numeros para a segunda caixa
particulas_caixa_B = random.sample(range(0,399), 10)
ds = 0.05
offset = 0.025
linhas_caixa_B = [offset + int(x/20)*ds for x in particulas_caixa_B]
colunas_caixa_B = [offset + x%20*ds for x in particulas_caixa_B]
axs[1].scatter(linhas_caixa_B, colunas_caixa_B)

# legenda indicando as caixas
axs[0].set_xlabel("Caixa A", fontsize=20)
axs[1].set_xlabel("Caixa B", fontsize=20)

# salvando:
plt.savefig("img-caixas.svg", format="svg")
plt.close()