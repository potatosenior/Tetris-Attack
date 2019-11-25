
//=========================================================
typedef struct TPacman Pacman;
typedef struct TPhantom Phantom;
typedef struct TCenario Cenario;
typedef struct SGrade StructGrade;

//=========================================================
void carregaTexturas();

//=========================================================
Cenario* cenario_carrega(char *arquivo);
void cenario_destroy(Cenario* cen);
void cenario_desenha(Cenario* cen);

//
StructGrade* criar_grade(int x, int y);
void destruir_grade(StructGrade *grade);
int grade_perdeu(StructGrade *grade);
void grade_movimenta(StructGrade *grade, Cenario *cen, int direcao);
void grade_desenha(StructGrade *grade);
void grade_AlteraDirecao(StructGrade *grade, int direcao);
void grade_mudar(Cenario *cen, StructGrade *gradee);

//=========================================================
Pacman* pacman_create(int x, int y);
void pacman_destroy(Pacman *pac);
int pacman_vivo(Pacman *pac);
void pacman_desenha(Pacman *pac);
void pacman_AlteraDirecao(Pacman *pac, int direcao, Cenario *cen);
void pacman_movimenta(Pacman *pac, Cenario *cen);

////=========================================================
Phantom* phantom_create(int x, int y);
void phantom_destroy(Phantom *ph);
void phantom_movimenta(Phantom *ph, Cenario *cen, Pacman *pac);
void phantom_desenha(Phantom *ph);
