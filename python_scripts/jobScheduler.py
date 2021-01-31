import os, sys
import json


#%%
# ler arquivo JSON
def lerJson(nomeDoArquivo: str):
    try:
        nomeDoArquivo = nomeDoArquivo + ".json"
        arquivo = open(nomeDoArquivo, 'r')
        dados = json.load(arquivo)
        return dados
    except:
        print('###\nErro na leitura do arquivo (%s)\n' % (nomeDoArquivo))
        os.system("pause")
        sys.exit(2)


#%%
# salvar arquivo JSON
def salvarJson(nomeDoArquivo: str, data):
    if len(data["nameJobs"]) != 0:
        output = "hasKeys"
        pass
    else:
        output = "hasNoKeys"
        data = ""
        pass
    try:
        nomeDoArquivo = nomeDoArquivo + ".json"
        arquivo = open(nomeDoArquivo, 'w')
        json.dump(data, arquivo, indent="    ")
        return output
    except:
        print('###\nErro ao salvar o arquivo (%s)\n' % (nomeDoArquivo))
        os.system("pause")
        sys.exit(2)


def main_func(ler=None):
    user_input = str(input("\nNome do arquivo (JSON):  "))
    #user_input = "ExemploArquivoJobSchedulerJSON"

    #ler arquivo json pela primeira vez
    arquivo_json = lerJson(user_input)
    #extrai primeiro trabalho
    nameJob = arquivo_json["nameJobs"].pop(0)
    worker = arquivo_json.pop(nameJob)
    #salva arquivo
    status = salvarJson(user_input, arquivo_json)

    #diretorio em que se encontra o json file
    parentFolder = os.getcwd()

    keyPass = True
    while keyPass == True:
        #muda de diretorio raiz
        print("\n\n")
        os.chdir(worker["cwd"])
        print(os.getcwd())
        #executa .bat
        try:
            comandoBat = worker["bat"] + " " + worker["args"]
        except:
            comandoBat = worker["bat"]
        os.system(comandoBat)
        if status == "hasNoKeys":
            keyPass = False
            pass
        else:
            os.chdir(parentFolder)
            arquivo_json = lerJson(user_input)
            nameJob = arquivo_json["nameJobs"].pop(0)
            worker = arquivo_json.pop(nameJob)
            status = salvarJson(user_input, arquivo_json)
            pass
        pass


# %%
if __name__ == "__main__":
    main_func()