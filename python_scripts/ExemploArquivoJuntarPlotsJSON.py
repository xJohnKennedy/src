### O arquivo Json não aceita comentarios
### apagar os comentarios bloco de comentario e renomear a extensao do arquivo para .json
""" 
===>>> inicio do bloco de comentario

### SYNTAXE DO ARQUIVOS JSON
{
    "nomeGrafico": nome do grafico,
    "nomeEixoY": nome do eixo e aceita linguagem Latex,
    "nomeEixoX": nome do eixo e aceita linguagem Latex,
    "nomePlots": lista com os arquivos que contem os dados dos graficos,
    "INSERIR_AQUI_NomeDoPlotComConfiguracaoAdicional": adicionar lista de argumentos a passar com as configuracoes adicionais,
    "LinhaVertical": {
        "plotar": valor TRUE ou FALSE,
        "local_x": 1.0,
        "kwargs": {
            "color": "black",
            "linewidth": 3
        }
    }
}
===>>> termino do bloco de comentario
"""
{
    "nomeGrafico":
    "BifurcacaoOOPP",
    "nomeEixoY":
    "$W_{8,1}/h$",
    "nomeEixoX":
    "$\\omega_{1}/\\omega_{0}$",
    "nomePlots": [
        "d1fae3cc6e25307050d18b422c8acc3a_C1_c_1",
        "d1fae3cc6e25307050d18b422c8acc3a_C1_c_2",
        "d1fae3cc6e25307050d18b422c8acc3a_C1_c_3",
        "d1fae3cc6e25307050d18b422c8acc3a_C1_c_4"
    ],
    "INSERIR_AQUI_NomeDoPlotComConfiguracaoAdicional": {
        "color": "yellow"
    },
    "LinhaVertical": {
        "plotar": true,
        "local_x": 1.0,
        "kwargs": {
            "color": "black",
            "linewidth": 3
        }
    }
}