def SeparaString(palavra: str):
    pontos = []
    palavraPath = ""
    # separa as virgulas pois esta delimitam tradutores diferentes
    tradutorVirgula = palavra.split(",")

    for i in tradutorVirgula:
        palavraPath = palavraPath + "_c%s" % (i)
        pass

    for trechoString in tradutorVirgula:
        if '$' in trechoString:
            tradutor = trechoString.split("-")
            nome = tradutor[0]
            #conta ocorrencias de $ na string formadora dos nomes
            count = 0
            for i in nome:
                if i == '$':
                    count = count + 1
            conjunto_gerador = [nome]
            conjunto_gerado = []
            for i in range(count):
                #obtem os codigos a substituir
                cod_subs = tradutor[i + 1]
                if '#' in cod_subs:
                    cod_subs = cod_subs.split("#")
                    cod_subs = range(int(cod_subs[0]), int(cod_subs[1]) + 1)
                for j in conjunto_gerador:
                    for k in cod_subs:
                        conjunto_gerado.append(j.replace('$', str(k), 1))
                conjunto_gerador = conjunto_gerado
                conjunto_gerado = []
            pontos.extend(conjunto_gerador)
            pass
        else:
            pontos.append(trechoString)
            pass
        pass

    return pontos, palavraPath
