
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
void tempo_desenha(StructGrade *gradee, int tempo);
void pontuacao_desenha(StructGrade *gradee);

//=======================FUNCOES===========================
void Salvar_high_score(StructGrade *gradee);
void Carregar_high_score(StructGrade *gradee);
void Alterar_score(StructGrade *gradee, int scoree);
void Alterar_high_score_grade(StructGrade *gradee, int scoree);
StructGrade* criar_grade(int x, int y);
void destruir_grade(StructGrade *grade);
int grade_perdeu(StructGrade *grade);
void grade_movimenta(StructGrade *grade, Cenario *cen, int direcao);
void grade_desenha(StructGrade *grade);
void grade_AlteraDirecao(StructGrade *grade, int direcao);
void grade_mudar(Cenario *cen, StructGrade *gradee);
