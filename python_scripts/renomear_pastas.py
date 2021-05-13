import os

renomear_de_f = 8

inicio_pasta_de_f = 1
fim_pasta_de_f = 41

renomear_para_f = 4
nova_pasta_para_f = 59

qtde = fim_pasta_de_f - inicio_pasta_de_f + 1

input("renomear de ponto f{:d}_{:d} para ponto f{:d}_{:d}".format(
    renomear_de_f, inicio_pasta_de_f, renomear_para_f, nova_pasta_para_f))

for i in reversed(range(qtde)):
    try:
        nome_pasta_antiga = "ponto f{:d}_{:d}".format(renomear_de_f,
                                                      inicio_pasta_de_f + i)
        #print(nome_pasta_antiga)
        nome_pasta_nova = "ponto f{:d}_{:d}".format(renomear_para_f,
                                                    nova_pasta_para_f + i)
        #print(nome_pasta_nova)
        os.rename(nome_pasta_antiga, nome_pasta_nova)
        pass
    except:
        pass