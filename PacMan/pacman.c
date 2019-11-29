#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <windows.h>
#include <gl/gl.h>
#include <time.h>
#include "SOIL.h"
#include "pacman.h"

//=========================================================
// Tamanho de cada bloco da matriz do jogo
#define bloco 70

// Tamanho da matriz do jogo
#define preto -1
#define L 14
#define C 6

// Tamanho de cada bloco da matriz do jogo na tela
#define TAM 0.13f
#define TAMF 2.0f              // tamanho da imagem de fundo 1.56f

#define TAMS 0.06f              // tamanho do placar
#define TAMIPV 0.10f             // tamanho da img do placar vertical
#define TAMIPH 0.29f             // tamanho da img do placar horizontalmente

#define TAMIPMV 0.12f             // tamanho da img do placar M vertical
#define TAMIPMH 0.29f             // tamanho da img do placar M horizontalmente

#define TAMITV 0.10f             // tamanho da img do tempo vertical
#define TAMITH 0.27f             // tamanho da img do tempo horizontalmente
                //0.5375f   0.57f   -0.1f
#define centro 0.6855f           // alinhar matriz com o fundo horizontalmente
#define centro_vertical 0.57f   // alinhar matriz com o fundo vertical
#define espacos 0.15f
#define espacog 0.29f           //alinhar as linhas pra nao se sobreporem

//Fun��es que convertem a linha e coluna da matriz em uma coordenada de [-1,1]
#define MAT2X(j) ((j)*0.1f-1)
#define MAT2Y(i) (0.9-(i)*0.1f)

// Dire��es
#define esquerda 0
#define baixo 1
#define direita 2
#define cima 3

//=========================================================
// Estruturas usadas para controlar o jogo

struct SGrade {
    int x,y;
    int status; //0 = perdeu
    int highscore;
    int pontuacao;
};

struct TCenario {
    int mapa[L][C];
};

//==============================================================
// Texturas
//==============================================================

GLuint grade;
GLuint cristais[5];
GLuint score[11];
GLuint fundo;
GLuint imagens[4];

static void desenhaSprite(float coluna,float linha, GLuint tex);
static GLuint carregaArqTextura(char *str);

// Fun��o que carrega todas as texturas do jogo
void carregaTexturas() {
    int i;
    char str[50];

    //imagem de fundo
    fundo = carregaArqTextura(".//Sprites//fundo.png");

    // Grade
    grade = carregaArqTextura(".//Sprites//grade.png");

    // Cristais
    for(i=0; i < 5; i++) {
        sprintf(str,".//Sprites//cristal%d.png",i);
        cristais[i] = carregaArqTextura(str);
    }

    // Pontuacao
    for(i=0; i < 11; i++) {
        sprintf(str,".//Sprites//score%d.png",i);
        score[i] = carregaArqTextura(str);
    }

    // Imagens
    for(i=0; i < 3; i++) {
        sprintf(str,".//Sprites//imagem%d.png",i);
        imagens[i] = carregaArqTextura(str);
    }
}

// Fun��o que carrega um arquivo de textura do jogo
static GLuint carregaArqTextura(char *str) {
    // http://www.lonesock.net/soil.html
    GLuint tex = SOIL_load_OGL_texture
                 (
                     str,
                     SOIL_LOAD_AUTO,
                     SOIL_CREATE_NEW_ID,
                     SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y |
                     SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
                 );

    /* check for an error during the load process */
    if(0 == tex) {
        printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
    }

    return tex;
}

// Fun��o que recebe uma linha e coluna da matriz e um c�digo
// de textura e desenha um quadrado na tela com essa textura
void desenhaSprite(float col,float linha, GLuint tex) {
    glColor3f(1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float coluna = col;

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f,0.0f); glVertex2f(coluna+centro, linha-centro_vertical);
        glTexCoord2f(1.0f,0.0f); glVertex2f(coluna+TAM+centro, linha-centro_vertical);
        glTexCoord2f(1.0f,1.0f); glVertex2f(coluna+TAM+centro, linha+TAM-centro_vertical);
        glTexCoord2f(0.0f,1.0f); glVertex2f(coluna+centro, linha+TAM-centro_vertical);
    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);

}

// Fun��o pra desenhar a imagem de fundo
void desenhaFundo(GLuint tex){
    glColor3f(1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float lin, col;
    lin = -1;
    col = -1;

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f,0.0f); glVertex2f(col, lin);
        glTexCoord2f(1.0f,0.0f); glVertex2f(col+TAMF, lin);
        glTexCoord2f(1.0f,1.0f); glVertex2f(col+TAMF, lin+TAMF);
        glTexCoord2f(0.0f,1.0f); glVertex2f(col, lin+TAMF);
    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);

}

void desenhaPlacar(GLuint tex, float col, float linha){
    glColor3f(1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);
    float lin;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //float lin;
    //lin = -0.15f;
    lin = linha;

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f,0.0f); glVertex2f(col, lin);
        glTexCoord2f(1.0f,0.0f); glVertex2f(col+TAMS, lin);
        glTexCoord2f(1.0f,1.0f); glVertex2f(col+TAMS, lin+TAMS);
        glTexCoord2f(0.0f,1.0f); glVertex2f(col, lin+TAMS);
    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}
void desenhaImagemPlacar(GLuint tex, float col){
    glColor3f(1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float lin;
    lin = -0.08f;


    glBegin(GL_QUADS);
        glTexCoord2f(0.0f,0.0f); glVertex2f(col, lin);
        glTexCoord2f(1.0f,0.0f); glVertex2f(col+TAMIPH, lin);
        glTexCoord2f(1.0f,1.0f); glVertex2f(col+TAMIPH, lin+TAMIPV);
        glTexCoord2f(0.0f,1.0f); glVertex2f(col, lin+TAMIPV);
    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

void desenhaMaiorPlacar(GLuint tex, float col, float linha){
    glColor3f(1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);
    float lin;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //float lin;
    //lin = 0.15f;
    lin = linha;

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f,0.0f); glVertex2f(col, lin);
        glTexCoord2f(1.0f,0.0f); glVertex2f(col+TAMS, lin);
        glTexCoord2f(1.0f,1.0f); glVertex2f(col+TAMS, lin+TAMS);
        glTexCoord2f(0.0f,1.0f); glVertex2f(col, lin+TAMS);
    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

void desenhaImagemMaiorPlacar(GLuint tex, float col){
    glColor3f(1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float lin;
    lin = 0.22f;

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f,0.0f); glVertex2f(col, lin);
        glTexCoord2f(1.0f,0.0f); glVertex2f(col+TAMIPMH, lin);
        glTexCoord2f(1.0f,1.0f); glVertex2f(col+TAMIPMH, lin+TAMIPMV);
        glTexCoord2f(0.0f,1.0f); glVertex2f(col, lin+TAMIPMV);
    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

void desenhaTempo(GLuint tex, float col, float linha){
    glColor3f(1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);
    float lin;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //float lin;
    //lin = 0.25f;
    lin = linha;

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f,0.0f); glVertex2f(col, lin);
        glTexCoord2f(1.0f,0.0f); glVertex2f(col+TAMS, lin);
        glTexCoord2f(1.0f,1.0f); glVertex2f(col+TAMS, lin+TAMS);
        glTexCoord2f(0.0f,1.0f); glVertex2f(col, lin+TAMS);
    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

void desenhaImagemTempo(GLuint tex, float col){
    glColor3f(1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float lin;
    lin = 0.67f;

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f,0.0f); glVertex2f(col, lin);
        glTexCoord2f(1.0f,0.0f); glVertex2f(col+TAMITH, lin);
        glTexCoord2f(1.0f,1.0f); glVertex2f(col+TAMITH, lin+TAMITV);
        glTexCoord2f(0.0f,1.0f); glVertex2f(col, lin+TAMITV);
    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}
//==============================================================
// Cenario
//==============================================================

// Fun��o que carrega os dados da matriz
Cenario* cenario_carrega() {
    int i,j, num;
    Cenario* cen = malloc(sizeof(Cenario));

    for(i=0; i< L; i++){
        for(j=0; j < C; j++){
            cen->mapa[i][j] = preto;
        }
    }

    for(i=13; i>8; i--)
        for(j=0; j<C; j++) {
            num = rand() % 5;
            if(j > 1)
                while(num == cen->mapa[i][j-2] && num == cen->mapa[i][j-1]){//impede de ter 3 iguais na mesma linha
                    num = rand() % 5;
                }
            cen->mapa[i][j] = num;
        }

        printf("cenario carregado!\n");
    return cen;
}

// Libera os dados associados ao cen�rio
void cenario_destroy(Cenario* cen) {
    free(cen);
    printf("Cenario destruido!\n");
}

// Percorre a matriz do jogo desenhando os sprites
void cenario_desenha(Cenario* cen) {
    int i,j;
    float espaco = -2.0, vertical = -3.8;
    srand(time(NULL));

    desenhaFundo(fundo);

    for(i=0; i<L; i++){
        for(j=0; j<C; j++) {
            if(cen->mapa[i][j] != preto && i != 0 && i != 1){
                desenhaSprite(MAT2X(j+espaco),MAT2Y(i+vertical),cristais[cen->mapa[i][j]]);
                
            }
        espaco += espacos+0.14;
        }
    espaco = 0.0;
    vertical += espacos+0.14;
    }
}

void pontuacao_desenha(StructGrade *gradee){
    int num[5], pontuacao;
    char numeros[5];
    pontuacao = gradee->pontuacao;
    float espacamento = 1.0;

    itoa(pontuacao, numeros, 10);

    for(int i = 4; i >= 0; i--)
    {
        num[i] = numeros[i] - 48;
        //printf("num[%i] = %d\n", i, num[i]);
        if(num[i]>=0){
            desenhaPlacar(score[num[i]],MAT2Y(espacamento), -0.15f);
            espacamento += 0.5;
        }
    }
    desenhaImagemPlacar(imagens[1], MAT2Y(3.15) );
}

void maior_pontuacao_desenha(StructGrade *gradee){
    int num[5], pontuacao;//, resto;
    char numeros[5];
    pontuacao = gradee->highscore;
    float espacamento = 1.0;
    //printf("highscore = %d\n", pontuacao);

    itoa(pontuacao, numeros, 10);
/*
    num[4] = pontuacao / 10000; resto = pontuacao % 10000;
    num[3] = resto / 1000; resto = resto % 1000;
    num[2] = resto / 100; resto = resto % 100;
    num[1] = resto / 10; resto = resto % 10;
    num[0] = resto;
*/
    for(int i = 4; i >= 0; i--)
    {
        num[i] = numeros[i] - 48;
        //printf("num[%i] = %d\n", i, num[i]);
        if(num[i]>=0){
            desenhaMaiorPlacar(score[num[i]],MAT2Y(espacamento), 0.15f);
            espacamento += 0.5;
        }
    }
    desenhaImagemMaiorPlacar(imagens[0], MAT2Y(3.15));
}

void tempo_desenha(int temp){
    int num[5], tempo;
    tempo = temp;
    //printf("time = %d\n", tempo);
    num[4] = (tempo % 60) % 10;     //segundos unidade
    num[3] = (tempo % 60) / 10;     //segundos dezena
    num[2] = 10;
    num[1] = (tempo / 60);          //minutos unidade
    num[0] = (tempo / 600);         //minutos dezena

    float espacamento = 15.6;

    for(int i = 4; i >= 0; i--)
    {
        //printf("num[%i] = %d\n", i, num[i]);
        if(num[i]>=0)
            desenhaTempo(score[num[i]],MAT2Y(espacamento), 0.6f);
        espacamento += 0.5;
    }
    desenhaImagemTempo(imagens[2], MAT2Y(17.6));
}

//Adiciona uma linha e sobe as demais
void adicionar_linha(Cenario *cen, StructGrade *gradee){
    for(int i = 0; i < 14; ++i){
        for(int j = 0; j < 6; ++j){
            int cristal = cen->mapa[i][j];

            //if(cristal != preto && i == 14) {
                //grade_morre(gradee);
            //    continue;
            //}

            if(cristal == -1) continue;
            cen->mapa[i-1][j] = cristal;
        }
    }

    for(int i = 0; i < 6; ++i){
        int num = rand() % 5;
        if(i > 1)
            while(num == cen->mapa[13][i-2] && num == cen->mapa[13][i-1]){//impede de ter 3 iguais na mesma linha
                num = rand() % 5;
            }
        cen->mapa[13][i] = num;
    }
    printf("linha adicionada!\n");
}

void descerBlocos(Cenario *cen){
    int cristal;

        for(int i = 1; i < 13; i++){ // linhas
            for(int j = 0; j < 6; j++){ //colunas
                cristal = cen->mapa[i][j];
                if(cristal == preto)
                    continue;

                if(cen->mapa[i+1][j] == preto){
                    cen->mapa[i+1][j] = cristal;
                    cen->mapa[i][j] = preto;
                }
            }
        }
}


void updateCristais(Cenario *cen, StructGrade *gradee){
    int sequencia, sequencia_h;
    //printf("funcao updateCristais!\n");

    //vertical - linha
    for(int i = 1; i < 12; i++){ // linhas
        for(int j = 0; j < 6; j++){ //colunas
            if(cen->mapa[i][j] == preto){
                continue;
            }

            sequencia = 1;
            while(cen->mapa[i+sequencia][j] == cen->mapa[i][j]){
                sequencia++;
            }

            if(sequencia >= 3){
                Alterar_score(gradee,(sequencia-2)*50);
                //printf("vertical sequencia = %d - j = %d, j + seq = %d ", sequencia, j, j+sequencia);
                for(int k = i; k < (i+sequencia); k++){
                    cen->mapa[k][j] = preto;
                    //printf("cristal[%i][%i] recebe preto!(j+seq = %d)\n", k, j, j+sequencia);
                }
                printf("Fez %d pontos vertical!\n", (sequencia-2)*50);
            }
        }
    }
    //horizontal - coluna
    for(int i = 1; i < 14; i++){ // linhas
        for(int j = 0; j < 4; j++){ //colunas
            if(cen->mapa[i][j] == preto){
                continue;
            }

            sequencia_h = 1;

            while(cen->mapa[i][j+sequencia_h] == cen->mapa[i][j]){
                sequencia_h++;
            }

            if(sequencia_h >= 3){
                Alterar_score(gradee,(sequencia_h-2)*50);

                for(int k = j; k < (j+sequencia_h); k++){
                    cen->mapa[i][k] = preto;
                    //printf("horizontal cristal[%i][%i] recebe preto!\n", i, k);
                }
                printf("Fez %d pontos horizontal!\n", (sequencia_h-2)*50);
            }
        }
    }
    //printf("fim da funcao updateCristais!\n");
}

//==============================================================
// Grade
//==============================================================

//inicializa a grade
StructGrade* criar_grade(int x, int y) {
    StructGrade* grade = malloc(sizeof(StructGrade));
    if(grade !=NULL) {
        grade->x = x;
        grade->y = y;
        grade->status = 1;
        grade->pontuacao = 0;
        grade->highscore = 0;
    }
    return grade;
    printf("grade criada!\n");
}

//destruir a grade
void destruir_grade(StructGrade *gradee) {
    free(gradee);
    printf("Grade destruido!\n");
}

// Alterar highscore da grade
void Alterar_high_score_grade(StructGrade *gradee, int scoree){
    gradee->highscore += scoree;
}

// Alterar score atual
void Alterar_score(StructGrade *gradee, int scoree){
    gradee->pontuacao += scoree;
}

// Salvar recordes
void Salvar_high_score(StructGrade *gradee){
    FILE *f;
    int record, scoree;

    scoree = gradee->highscore;

    f = fopen("highscore.bin", "rb");

    if (f == NULL){
        f = fopen("highscore.bin","w+b");
        fwrite(&scoree, sizeof(scoree), 1, f);
        fclose(f);
    }
    else
    {
        fclose(f);
        f = fopen("highscore.bin","r+b");
        fread(&record, sizeof(record), 1, f);
        fseek(f, 0,SEEK_SET);
        if(scoree > record){
            fwrite(&scoree, sizeof(scoree), 1, f);
            printf("High score salvo: %d\n", scoree);
        }
       fclose(f);
    }
}

// Ler recordes
void Carregar_high_score(StructGrade *gradee){
    FILE *f;
    int record;

    f = fopen("highscore.bin", "rb");

    if (f == NULL){ //nao tem nenhum salvo
        record = 0;
    }
    else
    {
        fclose(f);
        f = fopen("highscore.bin","r+b");
        fread(&record, sizeof(record), 1, f);
        printf("\nHigh score carregado: %d\n", record);
        fclose(f);
    }
    gradee->highscore = record;
}

// Verifica se o jogador ja perdeu
int grade_perdeu(StructGrade *gradee, Cenario *cen) {
    for(int i = 0; i < 6; i++)
        if(cen->mapa[1][i] != preto ){
            //printf("mapa[0][%i] = %d\n", i, cen->mapa[0][i]);
            grade_morre(gradee);
        }
    //printf("grade status = %d\n", gradee->status);
    return gradee->status;
}

//inverte os cristais selecionados
void grade_mudar(Cenario *cen, StructGrade *gradee) {
    int aux;

    aux = cen->mapa[gradee->y][gradee->x];
    cen->mapa[gradee->y][gradee->x] = cen->mapa[gradee->y][gradee->x+1];
    cen->mapa[gradee->y][gradee->x+1] = aux;
    //updateCristais(cen, gradee);
}

// atualizar a posicao da grade
void grade_movimenta(StructGrade *gradee, Cenario *cen, int direcao) {
    if(direcao == esquerda) {
        if(gradee->x - 1 < 0) return; //� fora da matriz
        else gradee->x -= 1;
    }else if(direcao == baixo) {
        if(gradee->y + 1 > 13) return; //� fora da matriz jogavel
        else gradee->y += 1;
    }else if(direcao == direita) {
        if(gradee->x + 1 > 4) return; //� fora da matriz
        else gradee->x += 1;
    }else if(direcao == cima) {
        if(gradee->y - 1 < 2) return;//� fora da matriz jogavel
        else gradee->y -= 1;
    }
}

//desenhar as grades
void grade_desenha(StructGrade *gradee) {
    float linha, coluna;

    linha = (int)gradee->y;
    coluna = (int)gradee->x;

    //linha = linha+(linha*1.16)-13*(espacos);
    linha = linha - 2.95f;//alinha a linha com o fundo
    linha = linha+(linha*espacog);//alinha as linhas pra nao se sobreporem

    if(gradee->status == 1) {
        desenhaSprite(MAT2X(coluna+(coluna*espacog)),MAT2Y(linha), grade);
        desenhaSprite(MAT2X(coluna+1.16+espacos+(coluna*espacog)),MAT2Y(linha), grade);
    }
}

void grade_morre(StructGrade *gradee) {
    gradee->status = 0;
    //printf("grade morreu!\n");
}
