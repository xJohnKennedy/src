# To add a new cell, type '# %%'
# To add a new markdown cell, type '# %% [markdown]'
# %%
import matplotlib.pyplot as pyplot
import numpy as np
import os, sys
import json

# %%
## template Jonathas Kennedy 26/03/2020
## código deve estar antes de criar figura pelo pyplot.figure(figsize=(10, 10))
## razão de tamnhos de fontes e ticks estabelecida para figuresize=10x10

pyplot.rcParams['xtick.labelsize'] = 30
pyplot.rcParams['ytick.labelsize'] = 30
pyplot.rcParams['xtick.major.size'] = 12
pyplot.rcParams['xtick.minor.size'] = 6
pyplot.rcParams['ytick.major.size'] = 12
pyplot.rcParams['ytick.minor.size'] = 6
pyplot.rcParams['ytick.minor.visible'] = True
pyplot.rcParams['xtick.minor.visible'] = True

pyplot.rcParams['axes.titlesize'] = 20
pyplot.rcParams['axes.labelpad'] = 20
pyplot.rcParams['axes.labelsize'] = 30

pyplot.rcParams['legend.fontsize'] = 16

### https://github.com/matplotlib/matplotlib/issues/5907#issuecomment-179001811
pyplot.rcParams['agg.path.chunksize'] = 10000


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


# %%
#https://stackoverflow.com/questions/48912527/how-to-join-two-matplotlib-figures
def join(ax, filename: str, data_plots, axins):
    data = carrega_plot(filename)

    method = data['method'].item()
    tipoPlot = getattr(ax, method)
    args = tuple(data['args'])
    kwargs = data['kwargs'].item()
    #configuracao do plot de acordo com os dados passados pelo arquivo json
    try:
        #caso o nome do arquivo possua .npz
        filename = filename.split(".")
        config_plot = data_plots[filename[0]]
        print(config_plot)
        for i in config_plot:
            kwargs.update({i: config_plot[i]})
    except:
        try:
            # se nao existir uma configuracao especifica para o plot, verifica se existe
            # uma configuracao para todos os plots
            config_plot = data_plots["all"]
            print(config_plot)
            for i in config_plot:
                kwargs.update({i: config_plot[i]})
        except:
            pass
        pass

    tipoPlot(*args, **kwargs)
    if axins != None:
        tipoPlot = getattr(axins, method)
        #print(tipoPlot)
        tipoPlot(*args, **kwargs)
        pass


def carrega_plot(nome):
    try:  #tenta carregar arquivo
        data = np.load(nome, allow_pickle=True)
    except:
        try:  #tenta carregar arquivo com a adicao do sufixo .npz
            data = np.load(nome + ".npz", allow_pickle=True)
        except:
            print(
                '###\nErro na leitura do arquivo (%s). Verificar o campo \"nomePlots\" no arquivo .json\n'
                % (nome))
            os.system("pause")
            sys.exit(2)

    return data


# %%
def gera_plot(files, data_plots):

    nomde_grafico = data_plots["nomeGrafico"]
    print(nomde_grafico)

    # matplotlib plot
    fig, ax = pyplot.subplots(figsize=(10, 10))

    #plot com zoom
    axins = None
    try:
        if data_plots["PlotComZoom"]["plotar"] == True:
            plt = data_plots["PlotComZoom"]
            axins = ax.inset_axes([plt["x0"], plt["y0"], plt["dx"], plt["dy"]])
            #print(axins)
    except:
        pass

    # funcao para juntar os plots
    for nomeArquivo in files:
        join(ax, nomeArquivo, data_plots, axins)

    # plota linha horizontal
    comandos = data_plots["LinhaVertical"]
    if (comandos["plotar"] == True):
        kwargs = comandos["kwargs"]
        ax.axvline(x=comandos["local_x"], **kwargs)

    # limite dos plots
    try:
        lista = ["lim_x_menor", "lim_x_maior", "lim_y_menor", "lim_y_maior"]
        limites = []
        comandos = data_plots["LimitePlots"]
        for i in lista:
            cm = comandos.pop(i, None)
            if cm != None and cm != "":
                limites.append(float(cm))
            else:
                limites.append(None)
                pass
        pyplot.xlim(limites[0], limites[1])
        pyplot.ylim(limites[2], limites[3])
    except:
        pass

    # limite dos plots PlotComZoom
    if axins != None:
        try:
            lista = [
                "lim_x_menor", "lim_x_maior", "lim_y_menor", "lim_y_maior"
            ]
            limites = []
            comandos = data_plots["PlotComZoom"]
            for i in lista:
                cm = comandos.pop(i, None)
                if cm != None and cm != "":
                    limites.append(float(cm))
                else:
                    limites.append(None)
                    pass

            axins.set_xlim(limites[0], limites[1])
            axins.set_ylim(limites[2], limites[3])
            for tick in axins.get_xticklabels():
                tick.set_fontsize(comandos["tamanhoFonte"])
            for tick in axins.get_yticklabels():
                tick.set_fontsize(comandos["tamanhoFonte"])

            ax.indicate_inset_zoom(axins)
        except:
            pass

    pyplot.ylabel(data_plots["nomeEixoY"])
    pyplot.xlabel(data_plots["nomeEixoX"])
    #pyplot.ticklabel_format(axis='both', style='sci', scilimits=(0, 0))
    pyplot.savefig("" + nomde_grafico + '.png', dpi=300, bbox_inches='tight')
    pyplot.savefig("" + nomde_grafico + '.svg', dpi=300, bbox_inches='tight')
    #pyplot.show()
    pyplot.cla()
    pyplot.clf()
    pyplot.close('all')


def main_func(ler=None):
    user_input = str(input("\nNome do arquivo (juntarPlots.json):  "))
    if user_input == '' or user_input == None:
        user_input = 'juntarPlots'
    user_input = user_input.split(".")
    arquivo_json = lerJson(user_input[0])
    nome_arquivos = arquivo_json["nomePlots"]
    gera_plot(nome_arquivos, arquivo_json)


# %%
if __name__ == "__main__":
    main_func()
