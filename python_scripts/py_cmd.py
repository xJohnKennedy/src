import secao_de_poicare_plots as poincare
import plano_fase_plots as planoFase
import resposta_no_tempo_plots as respostaNoTempo
import bacia_atracao as BaciaAtracao
import os, getopt, sys

if __name__ == "__main__":
    currentWorkingDirectory = os.getcwd()
    while (True):
        msg: str ="\n\n" + currentWorkingDirectory + ">>>>>" + "\n[OPCAO] <, ARGUMENTOS>\n" + \
            "[1]: plotar secao de Poincare <-p> = numero de pontos de poincare\n" + \
            "[2]: plotar plano fase <-f> = numero de pontos do plano fase\n" + \
            "[3]: plotar resposta no tempo <-t> = numero de pontos da resposta no tempo\n" + \
            "     t = -1 plotar todos os pontos\n" + \
            "[4]: plotar [1],[2]\n"+\
            "[5]: plotar [1],[2],[3]\n" + \
            "[6]: plotar bacia de atracao\n" + \
            "[7]: plotar bacia de atracao + [1],[2]\n"

        print(msg)
        user_input = str(input("\nExecutar:  "))
        if len(user_input) == 1:
            argv = []
        else:
            user_input, argv = user_input.split(",")
            argv = argv.split()

        user_input = int(user_input)

        #extrai opcoes de argumento
        short_argv = "-p:-f:-t:"
        try:
            opts, args = getopt.getopt(argv, short_argv, [])
        except getopt.GetoptError:
            print('NAO foi possivel extrair os argumentos passados')
            sys.exit(2)

        numPontosPoincare = None
        numPontosPlanoFase = None
        numPontosRespostaTempo = None

        for opt, arg in opts:
            if opt in ('-p'):
                numPontosPoincare = int(arg)
            elif opt in ('-f'):
                numPontosPlanoFase = int(arg)
            elif opt in ('-t'):
                numPontosRespostaTempo = int(arg)

        if user_input == 1:
            poincare.main_func(numPontosPoincare)
            os.system("pause")
        elif user_input == 2:
            planoFase.main_func(numPontosPlanoFase)
            os.system("pause")
        elif user_input == 3:
            respostaNoTempo.main_func(numPontosRespostaTempo)
            os.system("pause")
        elif user_input == 4:
            poincare.main_func(numPontosPoincare)
            planoFase.main_func(numPontosPlanoFase)
            os.system("pause")
        elif user_input == 5:
            poincare.main_func(numPontosPoincare)
            planoFase.main_func(numPontosPlanoFase)
            respostaNoTempo.main_func(numPontosRespostaTempo)
            os.system("pause")
        elif user_input == 6:
            BaciaAtracao.main_func()
        elif user_input == 7:
            BaciaAtracao.main_func(True)