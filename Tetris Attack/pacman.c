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
#define N 20
#define L 14
#define C 6
// Tamanho de cada bloco da matriz do jogo na tela
#define TAM 0.1f
#define centro 0.1f
//Funções que convertem a linha e coluna da matriz em uma coordenada de [-1,1]
#define MAT2X(j) ((j)*0.1f-1)
#define MAT2Y(i) (0.9-(i)*0.1f)
// Direções
#define esquerda 0
#define baixo 1
#define direita 2
#define cima 3

//=========================================================
// Estruturas usadas para controlar o jogo
struct TPoint {
    int x,y;
};

const struct TPoint direcoes[4] = {{1,0},{0,1},{-1,0},{0,-1}};

struct SGrade {
    int x,y;
    int status; //0 = perdeu
    int pontuacao;
};

struct TCenario {
    int mapa[L][C];
    int inicializa;

    int nro_pastilhas;
    int NV;
    struct TVertice *grafo;
};

//==============================================================
// Texturas
//==============================================================

GLuint pacmanTex2d[12];
GLuint phantomTex2d[12];
GLuint mapaTex2d[14];

GLuint grade;
GLuint cristais[5];
GLuint score[10];

static void desenhaSprite(float coluna,float linha, GLuint tex);
static GLuint carregaArqTextura(char *str);

// Função que carrega todas as texturas do jogo
void carregaTexturas() {
    int i;
    char str[50];

    //sprintf(str,".//Sprites//grade.png");
    grade = carregaArqTextura(".//Sprites//grade.png");


    for(i=0; i < 5; i++) {
        sprintf(str,".//Sprites//cristal%d.png",i);
        cristais[i] = carregaArqTextura(str);
    }

    for(i=0; i < 10; i++) {
        sprintf(str,".//Sprites//sprite%d.png",i);
        score[i] = carregaArqTextura(str);
    }
}

// Função que carrega um arquivo de textura do jogo
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

// Função que recebe uma linha e coluna da matriz e um código
// de textura e desenha um quadrado na tela com essa textura
void desenhaSprite(float coluna,float linha, GLuint tex) {
    glColor3f(1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f,0.0f);
    glVertex2f(coluna+centro, linha);
    glTexCoord2f(1.0f,0.0f);
    glVertex2f(coluna+TAM+centro, linha);
    glTexCoord2f(1.0f,1.0f);
    glVertex2f(coluna+TAM+centro, linha+TAM);
    glTexCoord2f(0.0f,1.0f);
    glVertex2f(coluna+centro, linha+TAM);
    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);

}

//==============================================================
// Cenario
//==============================================================

static int cenario_EhCruzamento(int x, int y, Cenario* cen);
static int cenario_VerificaDirecao(int mat[N][N], int y, int x, int direcao);
static void cenario_constroiGrafo(Cenario* cen);

// Função que carrega os dados do cenário de um arquivo texto
Cenario* cenario_carrega(char *arquivo) {
    int i,j, num;
    Cenario* cen = malloc(sizeof(Cenario));

    for(i=0; i< L; i++)
        for(j=0; j < C; j++)
            cen->mapa[i][j] = preto;

    for(i=14; i>8; i--)
        for(j=0; j<C; j++) {
            num = rand() % 5;
            cen->mapa[i][j] = num;
        }
    return cen;

}

// Libera os dados associados ao cenário
void cenario_destroy(Cenario* cen) {
    //free(cen->grafo);
    free(cen);
}

// Percorre a matriz do jogo desenhando os sprites
void cenario_desenha(Cenario* cen) {
    int i,j, num;
    srand(time(NULL));

    for(i=0; i<L; i++)
        for(j=0; j<C; j++) {
            if(cen->mapa[i][j] != preto)
                desenhaSprite(MAT2X(j),MAT2Y(i),cristais[cen->mapa[i][j]]);
        }
}

// Função que verifica se é possivel andar em uma determinada direção
static int cenario_VerificaDirecao(int mat[N][N], int y, int x, int direcao) {
    int xt = x;
    int yt = y;
    while(mat[yt + direcoes[direcao].y][xt + direcoes[direcao].x] == 0) { //não é parede...
        yt = yt + direcoes[direcao].y;
        xt = xt + direcoes[direcao].x;
    }

    if(mat[yt + direcoes[direcao].y][xt + direcoes[direcao].x] < 0)
        return -1;
    else
        return mat[yt + direcoes[direcao].y][xt + direcoes[direcao].x] - 1;
}

//==============================================================
// Grade
//==============================================================

static void grade_morre(StructGrade *grade);

//inicializa a grade
StructGrade* criar_grade(int x, int y) {
    StructGrade* grade = malloc(sizeof(StructGrade));
    if(grade !=NULL) {
        grade->x = x;
        grade->y = y;
        grade->status = 1;
    }
    return grade;
}

//destruir a grade
void destruir_grade(StructGrade *grade) {
    free(grade);
}

// Verifica se o jogador ja perdeu
int grade_perdeu(StructGrade *grade) {
    if (grade->status == 1) return 1;
    else return 0;
}

//inverte os cristais selecionados
void grade_mudar(Cenario *cen, StructGrade *gradee) {
    int aux;

    aux = cen->mapa[gradee->y][gradee->x];
    cen->mapa[gradee->y][gradee->x] = cen->mapa[gradee->y][gradee->x+1];
    cen->mapa[gradee->y][gradee->x+1] = aux;
}

// atualizar a posicao da grade
void grade_movimenta(StructGrade *grade, Cenario *cen, int direcao) {
    if(direcao == esquerda) {
        if(grade->x - 1 < 0) return; //é fora da matriz
        else grade->x -= 1;
    }else if(direcao == baixo) {
        if(grade->y + 1 > 13) return; //é fora da matriz jogavel
        else grade->y += 1;
    }else if(direcao == direita) {
        if(grade->x + 1 > 4) return; //é fora da matriz
        else grade->x += 1;
    }else if(direcao == cima) {
        if(grade->y - 1 < 1) return;//é fora da matriz jogavel
        else grade->y -= 1;
    }

    printf("\nGrade X:%d Y:%d", grade->x, grade->y);
}

//desenhar as grades
void grade_desenha(StructGrade *gradee) {
    float linha, coluna;

    linha = (int)gradee->y;
    coluna = (int)gradee->x;

    if(gradee->status == 1) {
        desenhaSprite(MAT2X(coluna),MAT2Y(linha), grade);
        desenhaSprite(MAT2X(coluna+1),MAT2Y(linha), grade);
    }
}

void grade_morre(StructGrade *grade) {
    if(grade->status == 1) grade->status = 0;
}
