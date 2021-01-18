# To add a new cell, type '# %%'
# To add a new markdown cell, type '# %% [markdown]'
# %%
import matplotlib.pyplot as pyplot
import numpy as np
import os
import pandas as pandas

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
variavel = 1
nomde_grafico = 'Plano fase W1 x dW3dt'
print(nomde_grafico)

# matplotlib plot
fig, ax = pyplot.subplots(figsize=(10, 10))

files = [
    '44c9f441e75plano_fase_C1.npz', '44c9f441e75plano_fase_C2.npz',
    '44c9f441e75plano_fase_C3.npz', '44c9f441e75plano_fase_C4.npz'
]

#join(ax, files)

# trecho periodico
data = np.load(files[0], allow_pickle=True)
method = getattr(ax, data['method'].item())
args = tuple(data['args'])
datax = args[0]
data = np.load(files[2], allow_pickle=True)
method = getattr(ax, data['method'].item())
args = tuple(data['args'])
datay = args[1]
kwargs = data['kwargs'].item()
kwargs.update({'color': 'blue'})
method(datax, datay, **kwargs)
""" pyplot.xlim(
    min(data_1__x) - 0.1 * abs(min(data_1__x)),
    max(data_1__x) + 0.1 * abs(max(data_1__x)))
pyplot.ylim(
    min(data_1__y) - 0.1 * abs(min(data_1__y)),
    max(data_1__y) + 0.1 * abs(max(data_1__y))) """

pyplot.ylabel(r'$W_{%i,%i}/h$' % (9, 1))
pyplot.xlabel(r'$W_{%i,%i}/h$' % (8, 1))
#pyplot.ticklabel_format(axis='both', style='sci', scilimits=(0, 0))
pyplot.savefig("" + nomde_grafico + '.png', dpi=300, bbox_inches='tight')
#pyplot.savefig("" + nomde_grafico + '.svg', dpi=300, bbox_inches='tight')
pyplot.show()
pyplot.cla()
pyplot.clf()
pyplot.close('all')
