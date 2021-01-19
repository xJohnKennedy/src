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
        if ".json" in nomeDoArquivo:
            pass
        else:
            nomeDoArquivo = nomeDoArquivo + ".json"
        arquivo = open("crossPlots\\" + nomeDoArquivo, 'r')
        dados = json.load(arquivo)
        return dados
    except:
        print('###\nErro na leitura do arquivo (%s)\n' % (nomeDoArquivo))
        os.system("pause")
        sys.exit(2)


# %%
#https://stackoverflow.com/questions/48912527/how-to-join-two-matplotlib-figures
def join(ax, files):
    data = [np.load(filename, allow_pickle=True) for filename in files]
    for datum in data:
        method = getattr(ax, datum['method'].item())
        args = tuple(datum['args'])
        kwargs = datum['kwargs'].item()
        method(*args, **kwargs)


# %%
def gera_plot(files, hashname: str):

    print(hashname)
    for crossPlot in files:
        variaveis_xy = []
        velEixo_xy = []
        nomde_grafico = 'CrossPlot ' + crossPlot
        print(nomde_grafico)
        #separa os nomes ex: C1xdC3dt => ['C1','dC3dt']
        crossPlotSplit = crossPlot.split("x")
        if len(crossPlotSplit) != 2:
            print('Nao e possivel plot cruzado de {:s}'.format(crossPlot))
            os.system("pause")
            sys.exit(2)
        print(crossPlotSplit)
        #trata os nomes para ficar somente a variavel ex: ['C1','dC3dt'] => ['C1','C3']
        for variavel in crossPlotSplit:
            if 'dt' in variavel:
                velEixo_xy.append(1)
            else:
                velEixo_xy.append(0)

            variavel = variavel.split("d")
            if len(variavel) > 1:
                variaveis_xy.append(variavel[1])
            else:
                variaveis_xy.append(variavel[0])
        print(variaveis_xy, velEixo_xy)

        # matplotlib plot
        fig, ax = pyplot.subplots(figsize=(10, 10))

        # cross Plot
        print(hashname)
        data = np.load("{1:s}plano_fase_{0:s}.npz".format(
            variaveis_xy[0], hashname),
                       allow_pickle=True)
        method = getattr(ax, data['method'].item())
        args = tuple(data['args'])
        datax = args[velEixo_xy[0]]
        data = np.load("{1:s}plano_fase_{0:s}.npz".format(
            variaveis_xy[1], hashname),
                       allow_pickle=True)
        method = getattr(ax, data['method'].item())
        args = tuple(data['args'])
        datay = args[velEixo_xy[1]]
        kwargs = data['kwargs'].item()
        kwargs.update({'color': 'blue'})
        method(datax, datay, **kwargs)

        # saving file to load in another python file
        np.savez("crossPlots\\" + nomde_grafico + '.npz',
                 method='plot',
                 args=(datax, datay),
                 kwargs=kwargs)

        pyplot.xlim(
            min(datax) - 0.01 * abs(min(datax)),
            max(datax) + 0.01 * abs(max(datax)))
        pyplot.ylim(
            min(datay) - 0.01 * abs(min(datay)),
            max(datay) + 0.01 * abs(max(datay)))

        if velEixo_xy[0] == 0:
            pyplot.xlabel(r'$%s/h$' % (variaveis_xy[0]))
        else:
            pyplot.xlabel(r'$\dot{%s/h}$' % (variaveis_xy[0]))

        if velEixo_xy[1] == 0:
            pyplot.ylabel(r'$%s/h$' % (variaveis_xy[1]))
        else:
            pyplot.ylabel(r'$\dot{%s/h}$' % (variaveis_xy[1]))

        #pyplot.ticklabel_format(axis='both', style='sci', scilimits=(0, 0))
        pyplot.savefig("crossPlots\\" + nomde_grafico + '.png',
                       dpi=300,
                       bbox_inches='tight')
        #pyplot.savefig("" + nomde_grafico + '.svg', dpi=300, bbox_inches='tight')
        #pyplot.show()
        pyplot.cla()
        pyplot.clf()
        pyplot.close('all')


def main_func(ler=None):
    #verifica diretorio corrente para execucao atraves do .bat de dentro do diretorio crossPlots
    cwd = os.getcwd()
    if cwd.find("crossPlots"):
        cwd = os.path.dirname(cwd)
        #print(cwd)
        os.chdir(cwd)
    obj = os.scandir()
    for entry in obj:
        if entry.is_file():
            name = entry.name
            if name.find("_PlanoFase_") != -1:
                break

    if name != None:
        hashname = name.split("_PlanoFase_")
        hashname = hashname[0]
    user_input = str(input("\nNome do arquivo (crossPlotsPlanoFase.json):  "))
    if user_input == '' or user_input == None:
        user_input = 'crossPlotsPlanoFase.json'
    arquivo_json = lerJson(user_input)
    nome_arquivos = arquivo_json["crossPlots"]
    #hashname = arquivo_json["hashname"]
    #print(nome_arquivos, hashname)
    gera_plot(nome_arquivos, hashname)


# %%
if __name__ == "__main__":
    main_func()
