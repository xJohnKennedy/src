import os

print("\nScript para renomear pasta do Espectro Carga-Frequencia")
print("\nPadrao da nomeclatura das pastas f$1_$2")
renomear_de_f = int(input("Renomear pasta f$1_##, onde $1 =: "))

inicio_pasta_de_f = int(
    input("Inicio da pasta f{}_$2, onde $2 =: ".format(renomear_de_f)))
fim_pasta_de_f = int(
    input("Fim da pasta f{}_$2, onde $2 =: ".format(renomear_de_f)))

renomear_para_f = int(input("Renomear para pasta f$1_##, onde $1 =: "))
inicio_nova_pasta_para_f = int(
    input("Inicio da pasta f{}_$2, onde $2 =: ".format(renomear_para_f)))
fim_nova_pasta_para_f = int(
    input("Fim da pasta f{}_$2, onde $2 =: ".format(renomear_para_f)))

qtde_pasta_de_f = fim_pasta_de_f - inicio_pasta_de_f
qtde_nova_pasta_para_f = fim_nova_pasta_para_f - inicio_nova_pasta_para_f

if abs(qtde_pasta_de_f) != abs(qtde_nova_pasta_para_f):
    raise Exception("Quantidade de pastas diferentes que serao renomeadas")

input("\nrenomear de ponto f{:d}_{:d} para ponto f{:d}_{:d}".format(
    renomear_de_f, inicio_pasta_de_f, renomear_para_f,
    inicio_nova_pasta_para_f))

direcao_de_f = 1
if qtde_pasta_de_f < 0:
    direcao_de_f = -1

direcao_para_f = 1
if qtde_nova_pasta_para_f < 0:
    direcao_para_f = -1

contador_pasta_de_f = range(
    inicio_pasta_de_f,
    inicio_pasta_de_f + direcao_de_f * (abs(qtde_pasta_de_f) + 1),
    direcao_de_f)
contador_pasta_para_f = iter(
    range(
        inicio_nova_pasta_para_f, inicio_nova_pasta_para_f + direcao_para_f *
        (abs(qtde_nova_pasta_para_f) + 1), direcao_para_f))

for i in contador_pasta_de_f:
    try:
        nome_pasta_antiga = "ponto f{:d}_{:d}".format(renomear_de_f, i)
        nome_pasta_nova = "ponto f{:d}_{:d}".format(
            renomear_para_f, next(contador_pasta_para_f))
        print(nome_pasta_antiga + " ===> " + nome_pasta_nova)
        os.rename(nome_pasta_antiga, nome_pasta_nova)
        pass
    except:
        pass

os.system("pause")