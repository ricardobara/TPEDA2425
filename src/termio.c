/**
 * @file termio.c
 * @author Ricardo Araújo (a21150@alunos.ipca.pt)
 * @brief Ficheiro que gere as funções de input e output no terminal do projeto.
 * @version 0.1
 * @date 18-05-2025
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "includes.h"

#pragma region Antenas

/**
 * @brief Pede as coordenadas de uma antena e se for possivel, cria e insere essa antena.
 * 
 * @param grafoHead Inicio da lista das antenas.
 * @param matriz Dados da matriz.
 * @return Antena* Inicio da lista atualizada.
 */
Antena* CriaInsereAntena(Antena* grafoHead, DadosMatriz matriz) {
    Coordenadas coorTemp = PedeCoordenadas("Coordenadas:\n");

    if (EncontraAntena(grafoHead, coorTemp.x, coorTemp.y) || !CabeNaMatriz(matriz, coorTemp.x, coorTemp.y)) {
        printf("Coordenadas Invalidas!\n");
        return grafoHead;
    }

    DadosAntena dadosTemp;
    dadosTemp.x = coorTemp.x;
    dadosTemp.y = coorTemp.y;

    printf("frequencia: ");
    LimpaBuffer();
    scanf("%c", &dadosTemp.freq);

    if (!ValidaCharAntena(dadosTemp.freq)) {
        printf("frequencia invalida!\n");
        return grafoHead;
    }

    Antena* aux = CriaAntena(dadosTemp);
    grafoHead = InsereAntenaOrdenado(grafoHead, aux);

    return grafoHead;
}

/**
 * @brief Pede as coordenadas de uma antena e se for possivel, remove essa antena.
 * 
 * @param grafoHead Inicio da lista das antenas.
 * @return Antena* Inicio da lista atualizada.
 */
Antena* PerguntaRemoveAntena(Antena* grafoHead) {
    if (!grafoHead) {
        printf("não existe uma lista!\n");
        return NULL;
    }

    Coordenadas coorTemp = PedeCoordenadas("que antena pretende remover?\n");
    Antena* aux = EncontraAntena(grafoHead, coorTemp.x, coorTemp.y);

    if (!aux) {
        printf("não existe nenhuma antena com essas coordenadas!\n");
        return grafoHead;
    }

    RemoveOutrasAdj(grafoHead, aux);
    RemoveOutrosNefastos(grafoHead, aux);
    grafoHead = RemoveAntena(grafoHead, aux);

    printf("antena removida com sucesso!\n");
    
    return grafoHead;
}

/**
 * @brief Se for possivel, altera as informações de uma antena.
 * 
 * @param grafoHead Inicio da lista das antenas.
 * @param matriz Dados da matriz.
 * @return Antena* Inicio da lista atualizada.
 */
Antena* AlteraAntena(Antena* grafoHead, DadosMatriz matriz) {
    if (!grafoHead) {
        printf("não existe uma lista!\n");
        return NULL;
    }

    Coordenadas coorTemp = PedeCoordenadas("que antena pretende alterar?");
    Antena* aux = EncontraAntena(grafoHead, coorTemp.x, coorTemp.y);

    if (!aux) {
        printf("não existe nenhuma antena com essas coordenadas!\n");
        return grafoHead;
    }

    coorTemp = PedeCoordenadas("Novos dados da antena:");
    if (!CabeNaMatriz(matriz, coorTemp.x, coorTemp.y) || EncontraAntena(grafoHead, coorTemp.x, coorTemp.y)) {
        printf("coordenadas invalidas!\n");
        return grafoHead;
    }

    char freqTemp;
    printf("frequencia: ");
    LimpaBuffer();
    scanf("%c", &freqTemp);
    printf("\n");

    if (!ValidaCharAntena(freqTemp)) {
        printf("frequencia invalida!\n");
        return grafoHead;
    }

    RemoveOutrasAdj(grafoHead, aux);
    RemoveOutrosNefastos(grafoHead, aux);
    aux->adjHead = DestroiAdj(aux->adjHead);
    aux->nefHead = DestroiNefasto(aux->nefHead);

    // atribuir novos valores
    aux->dados.x = coorTemp.x;
    aux->dados.y = coorTemp.y;
    aux->dados.freq = freqTemp;

    GeraAdj(grafoHead);
    GeraNefasto(grafoHead, matriz);

    printf("dados alterados com sucesso!\n");

    return grafoHead;
}

/**
 * @brief Mostra a informação de todas as antenas presentes no grafo.
 * 
 * @param grafoHead Inicio da lista das antenas.
 * @return true Se existe pelo menos uma antena.
 * @return false Se não existe grafo.
 */
bool MostraListaAntenas(Antena* grafoHead) {
    // não ha nada para mostrar
    if (!grafoHead) return false;

    Antena* aux = grafoHead;
    int contador = 1;

    while (aux) {
        printf("[%d]: %c --> x: %d, y: %d\n", contador++, aux->dados.freq, aux->dados.x, aux->dados.y);
        aux = aux->next;
    }

    return true;
}

/**
 * @brief Mostra no terminal a matriz sem o efeito nefasto.
 * 
 * @param grafoHead Inicio da lista das antenas.
 * @param matriz Dados da matriz.
 * @return true Se conseguiu mostrar com sucesso.
 * @return false Se os dados da matriz são invalidos.
 */
bool MostraMatrizAntenas(Antena* grafoHead, DadosMatriz matriz) {
    if (matriz.linhas <= 0 || matriz.colunas <= 0) return false;

    // mostrar o numero de linhas
    printf("  ");
    for (int n = 1; n <= matriz.colunas; n++) {
        if (n < 10) printf("  %d", n);
        else if (n >= 10 && n < 100) printf(" %d", n);
        else printf("  ?");
    }
    printf("\n");

    for (int i = 1; i <= matriz.linhas; i++) {

        if (i < 10) printf(" %d", i);
        else if (i >= 10 && i < 100) printf("%d", i);
        else printf(" ?");

        for (int j = 1; j <= matriz.colunas; j++) {
            Antena* aux = EncontraAntena(grafoHead, i, j);

            char temp = VAZIO;
            if (aux) temp = aux->dados.freq;
            printf("  %c", temp);
        }
        printf("\n");
    }
    puts("\n");

    return true;
}

#pragma endregion
#pragma region Adj

/**
 * @brief Mostra a lista de adjuntas de uma antena.
 * 
 * @param alvo Antena que pretendemos mostrar a lista de adj.
 * @return true Se conseguiu mostrar.
 * @return false Se não existe grafo ou lista de adjuntas.
 */
bool MostraListaAdj(Antena* alvo) {
    if (!alvo || !alvo->adjHead) return false;

    Adj* aux = alvo->adjHead;
    int contador = 1;

    printf("Todas as Adjacencias de (%c) --> x: %d, y: %d\n", alvo->dados.freq, alvo->dados.x, alvo->dados.y);
    while (aux) {
        printf("  [%d] %c --> x: %d, y: %d\n", contador++, aux->dados.freq, aux->dados.x, aux->dados.y);
        aux = aux->next;
    }

    return true;
}

/**
 * @brief Pede a informação de uma antena e mostra as suas adjacências no terminal.
 * 
 * @param grafoHead Inicio da lista das antenas.
 * @return true Se conseguiu mostrar.
 * @return false Se não foi possivel mostrar.
 */
bool EscolheMostraAdj(Antena* grafoHead) {
    if (!grafoHead) return false;

    Coordenadas coorTemp = PedeCoordenadas("De que antena pretende mostrar as Adjacencias?");
    Antena* aux = EncontraAntena(grafoHead, coorTemp.x, coorTemp.y);

    if (!aux) {
        printf("não existe uma antena com essas coordenadas!\n");
        return false;
    }

    if (!MostraListaAdj(aux)) {
        printf("essa antena não tem efeitos nefastos!\n");
        return false;
    }

    return true;
}

#pragma endregion
#pragma region Nefasto

/**
 * @brief Mostra a lista do efeito nefasto de uma antena pretendida.
 * 
 * @param alvo Antena a mostrar o efeito nefasto.
 * @return true Se consegui mostrar.
 * @return false Se não existe grafo ou lista de nefasto da antena pretendida.
 */
bool MostraListaNefasto(Antena* alvo) {
    if (!alvo || !alvo->nefHead) return false;

    Nefasto* aux = alvo->nefHead;
    int contador = 1;

    printf("Todas os Efeitos Nefastos de (%c) --> x: %d, y: %d\n", alvo->dados.freq, alvo->dados.x, alvo->dados.y);
    while (aux) {
        printf("  [%d] %c --> x: %d, y: %d\n", contador++, NEF, aux->dados.x, aux->dados.y);
        aux = aux->next;
    }

    return true;
}

/**
 * @brief Pede a informação de uma antena e mostra a sua lista de efeito nefasto no terminal.
 * 
 * @param grafoHead Inicio da lista das antenas.
 * @return true Se conseguiu mostrar.
 * @return false Se não foi possivel mostrar.
 */
bool EscolheMostraNefasto(Antena* grafoHead) {
    if (!grafoHead) return false;

    Coordenadas coorTemp = PedeCoordenadas("De que antena quer mostrar o efeito nefasto?");
    Antena* aux = EncontraAntena(grafoHead, coorTemp.x, coorTemp.y);

    if (!aux) {
        printf("não existe uma antena com essas coordenadas!\n");
        return false;
    }

    if (!MostraListaNefasto(aux)) {
        printf("essa antena não tem efeitos nefastos!\n");
        return false;
    }

    return true;
}

/**
 * @brief Mostra a lista de efeito nefasto de todas as antenas do grafo.
 * 
 * @param grafoHead Inicio da lista das antenas.
 * @return true Se conseguiu mostrar.
 * @return false Se não existe grafo.
 */
bool MostraTodosNefastos(Antena* grafoHead) {
    if (!grafoHead) return false;

    Antena* aux = grafoHead;
    while (aux) {
        MostraListaNefasto(aux);
        printf("\n");

        aux = aux->next;
    }

    return true;
}

/**
 * @brief Mostra no terminal a matriz completa.
 * 
 * @param grafoHead Inicio da lista das antenas.
 * @param matriz Dados da matriz.
 * @return true Se conseguiu mostrar com sucesso.
 * @return false Se os dados da matriz são invalidos.
 */
bool MostraMatrizNefasto(Antena* grafoHead, DadosMatriz matriz) {
    if (matriz.linhas <= 0 || matriz.colunas <= 0) return false;

    // mostrar o numero de linhas
    printf("  ");
    for (int n = 1; n <= matriz.colunas; n++) {
        if (n < 10) printf("  %d", n);
        else if (n >= 10 && n < 100) printf(" %d", n);
        else printf("  ?");
    }
    printf("\n");

    for (int i = 1; i <= matriz.linhas; i++) {

        if (i < 10) printf(" %d", i);
        else if (i >= 10 && i < 100) printf("%d", i);
        else printf(" ?");

        for (int j = 1; j <= matriz.colunas; j++) {
            Antena* aux = EncontraAntena(grafoHead, i, j);
            bool condTemp = ExisteNefasto(grafoHead, i, j);

            char temp = VAZIO;
            if (aux && condTemp) temp = SBP;
            else if (aux) temp = aux->dados.freq;
            else if (condTemp) temp = NEF;
            printf("  %c", temp);
        }
        printf("\n");
    }
    puts("\n");

    return true;
}

#pragma endregion
#pragma region Matriz e Ficheiro

/**
 * @brief Lê uma matriz presente num ficheiro de texto.
 * 
 * @param ficheiro Nome do ficheiro.
 * @param grafoHead Inicio da lista das antenas.
 * @param matriz Dados da matriz.
 * @return Antena* Inicio da lista atualizada.
 */
Antena* LerFicheiroMatriz(char* ficheiro, Antena* grafoHead, DadosMatriz* matriz) {
    FILE* fp = fopen(ficheiro, "rt");
    // se não conseguimos abrir o ficheiro, devolvemos a lista tal como estava
    if (!fp) return grafoHead;

    // destuir uma lista, para criar uma nova
    grafoHead = DestroiAntenas(grafoHead);

    matriz->linhas = 0;
    matriz->colunas = 0;
    int colunasAtual = 0;
    char tempChar;

    // pode vir a ser necessario
    // LimpaBuffer();

    while ((tempChar = fgetc(fp)) != EOF) {
        if (tempChar == END_LINE) {
            if (matriz->linhas < MAX_MATRIZ) matriz->linhas++;

            if (matriz->colunas < colunasAtual)
                matriz->colunas = colunasAtual;
                
            colunasAtual = 0;
        }
        else if (colunasAtual < MAX_MATRIZ) {
            colunasAtual++;

            // acho que não é preciso verificar se já existe uma antena com as coordenadas atuais,
            // porque estámos a criar uma lista nova
            if (ValidaCharAntena(tempChar)) {
                DadosAntena tempDados;
                tempDados.x = matriz->linhas + 1;
                tempDados.y = colunasAtual;
                tempDados.freq = tempChar;

                Antena* aux = CriaAntena(tempDados);
                grafoHead = InsereAntenaOrdenado(grafoHead, aux);
            }
        }
    }

    // verificar se a ultima linha não foi contada (caso o ficheiro não termine com '\n')
    if (colunasAtual > 0 && matriz->linhas < MAX_MATRIZ) {
        matriz->linhas++;

        if (matriz->colunas < colunasAtual)
            matriz->colunas = colunasAtual;
    }
    fclose(fp);

    return grafoHead;
}

/**
 * @brief Lê os dados do grafo presentes num ficheiro binario.
 * 
 * @param ficheiro Nome do ficheiro.
 * @param grafoHead Inicio da lista das antenas.
 * @param matriz Dados da matriz.
 * @return Antena* Inicio da lista atualizada.
 */
Antena* LerAntenasBin(char* ficheiro, Antena* grafoHead, DadosMatriz matriz) {
    FILE* fp = fopen(ficheiro, "rb");
    if (!fp) return grafoHead;

    grafoHead = DestroiAntenas(grafoHead);

    DadosAntena dadosTemp;
    while (fread(&dadosTemp, sizeof(DadosAntena), 1, fp) == 1) {
        if (CabeNaMatriz(matriz, dadosTemp.x, dadosTemp.y)) {
            Antena* aux = CriaAntena(dadosTemp);
            grafoHead = InsereAntenaOrdenado(grafoHead, aux);
        }
    }
    fclose(fp);

    return grafoHead;
}

/**
 * @brief Guarda a matriz e as antenas num ficheiro de texto.
 * 
 * @param ficheiro Nome do ficheiro.
 * @param grafoHead Inicio da lista das antenas.
 * @param matriz Dados da matriz.
 * @return true Se conseguiu guardar.
 * @return false Se não conseguiu guardar.
 */
bool GuardarFicheiroMatriz(char* ficheiro, Antena* grafoHead, DadosMatriz matriz) {
    if (!ValidaMatriz(matriz)) return false;

    FILE* fp = fopen(ficheiro, "wt");
    if (!fp) return false;

    for (int i = 1; i <= matriz.linhas; i++) {
        for (int j = 1; j <= matriz.colunas; j++) {
            Antena* aux = EncontraAntena(grafoHead, i, j);

            char temp = VAZIO;
            if (aux) temp = aux->dados.freq;
            fprintf(fp, "%c", temp);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);

    return true;
}

/**
 * @brief Guarda os dados do grafo num ficheiro binario.
 * 
 * @param ficheiro Nome do ficheiro.
 * @param grafoHead Inicio da lista das antenas.
 * @return true Se conseguiu guardar.
 * @return false Se não conseguiu guardar.
 */
bool GuardarAntenasBin(char* ficheiro, Antena* grafoHead) {
    if (!grafoHead) return false;

    FILE* fp = fopen(ficheiro, "wb");
    if (!fp) return false;

    Antena* aux = grafoHead;
    while (aux) {
        if (fwrite(&(aux->dados), sizeof(DadosAntena), 1, fp) != 1) {
            fclose(fp);
            return false;
        }
        aux = aux->next;
    }
    fclose(fp);

    return true;
}

/**
 * @brief Cria uma matriz com as dimensões escolhidas pelo utilizador.
 * 
 * @param matriz Dados da matriz.
 * @return DadosMatriz Matriz atualizada.
 */
DadosMatriz CriaMatriz(DadosMatriz matriz) {
    DadosMatriz temp;
    printf("linhas: ");
    scanf("%d", &temp.linhas);
    printf("colunas: ");
    scanf("%d", &temp.colunas);

    if (!ValidaMatriz(temp)) {
        matriz.linhas = 10;
        matriz.colunas = 10;

        return matriz;
    }

    matriz = temp;
    return matriz;
}

/**
 * @brief Pede umas coordenadas ao utilizador.
 * 
 * @param mensagem Mensagem para o utilizador.
 * @return Coordenadas que o utilizador escolheu.
 */
Coordenadas PedeCoordenadas(char* mensagem) {
    Coordenadas aux;

    printf("%s\n", mensagem);
    printf("x: ");
    scanf("%d", &aux.x);
    printf("y: ");
    scanf("%d", &aux.y);
    printf("\n");

    return aux;
}

#pragma endregion
