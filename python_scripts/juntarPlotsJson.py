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
nomde_grafico = 'bifurcacao C' + str(variavel)
print(nomde_grafico)

# matplotlib plot
fig, ax = pyplot.subplots(figsize=(10, 10))

files = [
    'cbadee92a5c10baaffb58296b75cda0b_C1_c_1.npz',
    'cbadee92a5c10baaffb58296b75cda0b_C1_c_2.npz',
    '14f5d2e0ab6ac702250b31999825c353_C1_c_1.npz',
    '14f5d2e0ab6ac702250b31999825c353_C1_c_2.npz'
]

#join(ax, files)

data = np.load(files[0], allow_pickle=True)
method = getattr(ax, data['method'].item())
args = tuple(data['args'])
kwargs = data['kwargs'].item()
method(*args, **kwargs)

data = np.load(files[1], allow_pickle=True)
method = getattr(ax, data['method'].item())
args = tuple(data['args'])
kwargs = data['kwargs'].item()
method(*args, **kwargs)

data = np.load(files[2], allow_pickle=True)
method = getattr(ax, data['method'].item())
args = tuple(data['args'])
kwargs = data['kwargs'].item()
kwargs.update({'color': 'gray'})
method(*args, **kwargs)

data = np.load(files[3], allow_pickle=True)
method = getattr(ax, data['method'].item())
args = tuple(data['args'])
kwargs = data['kwargs'].item()
kwargs.update({'color': 'gray'})
method(*args, **kwargs)

pyplot.xlim(0.8, 1.2)
"""pyplot.ylim(
    min(data_1__y) - 0.1 * abs(min(data_1__y)),
    max(data_1__y) + 0.1 * abs(max(data_1__y))) """
pyplot.ylabel(r'$W_{' + str(variavel + 7) + ',1}/h$')
pyplot.xlabel(r'$\omega_{1}/\omega_{0}$')
#pyplot.ticklabel_format(axis='both', style='sci', scilimits=(0, 0))
pyplot.savefig("" + nomde_grafico + '.png', dpi=300, bbox_inches='tight')
pyplot.savefig("" + nomde_grafico + '.svg', dpi=300, bbox_inches='tight')
#pyplot.show()
pyplot.cla()
pyplot.clf()
pyplot.close('all')
