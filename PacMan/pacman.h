
//=======================ESTRUTURAS========================
typedef struct TCenario Cenario;
typedef struct SGrade StructGrade;

//=======================TEXTURAS==========================
void carregaTexturas();

//=======================CENARIO===========================
Cenario* cenario_carrega();
void cenario_destroy(Cenario* cen);
void cenario_desenha(Cenario* cen);
void pontuacao_desenha(StructGrade *gradee);
void tempo_desenha(int temp);
void pontuacao_desenha(StructGrade *gradee);
void pagInicial_desenha();
void pagFinal_desenha(int pag);
void desenhaImagemCreditos(GLuint tex, float col);

//=======================FUNCOES===========================
void adicionar_linha(Cenario *cen, StructGrade *gradee);
void descerBlocos(Cenario *cen);
void updateCristais(Cenario *cen, StructGrade *gradee);
void Salvar_high_score(StructGrade *gradee);
void Carregar_high_score(StructGrade *gradee);
void Alterar_score(StructGrade *gradee, int scoree);
void Alterar_high_score_grade(StructGrade *gradee, int scoree);
StructGrade* criar_grade(int x, int y);
void destruir_grade(StructGrade *grade);
int grade_perdeu(StructGrade *gradee, Cenario *cen);
void grade_morre(StructGrade *gradee);
void grade_movimenta(StructGrade *grade, Cenario *cen, int direcao);
void grade_desenha(StructGrade *grade);
void grade_AlteraDirecao(StructGrade *grade, int direcao);
void grade_mudar(Cenario *cen, StructGrade *gradee);
