//�R�}���h�v�����v�g�œ���\

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#define MAX_GRID 20 // �ő�}�X��
#define MIN_GRID 5 // �ŏ��}�X��
#define INT 0 // int�^��\���t���O
#define FLOAT 1 // float�^��\���t���O

typedef struct
{
    int row;//�ǂ��܂܂Ȃ�
    int column;//�ǂ��܂܂Ȃ�
    int wrow;//�ǂ��܂�
    int wcolumn;//�ǂ��܂�
    int bomb;//�ݒ肵���n����
}Setup;//�Ֆʂ̏����ݒ�

typedef struct
{
    int isopen; // �}�X�̊J�t���O
    int isflag; // �������Ă��Ă��邩�t���O
    int isbomb; // �n���t���O
    int ischeck; // �T���t���O
}Grid;//�e�}�X�����t���O

typedef struct
{
    int isdebug; // �f�o�b�O���[�h��
    int isfin; // �Q�[�����I���������i�O�F�f�t�H���g�@�P�F�Q�[���N���A�@�Q�F�Q�[���I�[�o�[�j
    int isretry; // ���g���C���邩�i�O�F�f�t�H���g�@�P�F��Փx��ς��ă��g���C�@�Q�F��Փx��ς����Ƀ��g���C�@�R�F�I���j
    int sum_flag;//�S�̂̊��̐�
}Manager;//�Q�[���̐i�s�󋵂ƃQ�[���S�̂̃J�E���^�[

typedef struct
{
    int act_menu; // �s���I���i�P�F�J����@�Q�F���j
    int select_X; // �I��x���W
    int select_Y; // �I��y���W
}Act;

// �v���g�^�C�v�錾
Grid** setup_grd(Setup*);
void input_area(Setup*);
void input_bomb(Setup*);
void init_grd(Setup*, Grid**);
void put_bomb(Setup*, Grid**);
void output_gameview(Setup*, Grid**, Manager*);
void output_fin(Manager*);
void input_act(Setup*,Grid**, Act*);
void update_grd(Setup*, Grid**, Manager*, Act*);
void rec_open(Setup*, Grid**);
void game_fin(Setup*, Grid**, Manager*, Act*);
char* itoJapan(int);
int count_bomb(int, int, Setup*, Grid**);
int error_check(char*, int, float, float);
int str_check(char*, int);
void title_scene(Manager*);
void debug(Setup*, Grid**, Manager*);
void cls(int);

char tmp[100];

int main()
{
    Setup stp;
    Grid** grd;
    Grid* base_grd;
    Manager mng = {0, 0, 0, 0};
    Act act;
    int i, j;

    title_scene(&mng);

    // �Q�[���S�̂̃��[�v
    while(1)
    {
        mng.isfin = 0;
        mng.sum_flag = 0;
        srand(time(NULL));

        if(mng.isretry != 2) grd = setup_grd(&stp);
        mng.isretry = 0;
        init_grd(&stp, grd);

        cls(mng.isdebug);

        // �P�^�[���̃��[�v
        while(1)
        {
            output_gameview(&stp, grd, &mng);

            debug(&stp, grd, &mng);

            output_fin(&mng);
            if(mng.isretry != 0) break;
            input_act(&stp, grd, &act);
            update_grd(&stp, grd, &mng, &act);
            game_fin(&stp, grd, &mng, &act);

            cls(mng.isdebug);
        }

        if(mng.isretry == 1)
        {
            free(base_grd);
            free(grd);
        }

        cls(mng.isdebug);
    }
    return 0;
}

// �Q�����z����쐬
Grid** setup_grd(Setup* pstp)
{
    Grid** grd;
    Grid* base_grd;
    int i, j;

    input_area(pstp);
    input_bomb(pstp);

    grd = (Grid**)malloc(sizeof(Grid*) * pstp->wrow);
    base_grd = (Grid*)malloc(sizeof(Grid) * pstp->wrow * pstp->wcolumn);
    //�ǂ��܂߂��T�C�Y
    for(i = 0; i < pstp->wrow; i++)
    {
        grd[i] = base_grd + i * pstp->wcolumn;
        //1��ڂ̊e�v�f���e�s�̐擪�Ƃ݂Ȃ��Đ擪�A�h���X�������Ă���
    }
    return grd;
}

void init_grd(Setup* pstp, Grid** grd)
{
    int i, j;
    // �e�}�X�̃t���O��������
    for(i = 0; i < pstp->wrow; i++)
    {
        for(j = 0; j < pstp->wcolumn; j++)
        {
            if
            (
                i == 0
                || i == pstp->wrow - 1
                || j == 0
                || j == pstp->wcolumn - 1
            ) grd[i][j].isopen = 2;
            else grd[i][j].isopen = 0; 
            //�ǂ�2, �����łȂ��}�X��0�ŏ�����

            grd[i][j].isflag = 0;
            grd[i][j].isbomb = 0;
            grd[i][j].ischeck = 0;
        }
    }
    put_bomb(pstp, grd);
}

// �}�X�̍L�������߂�
void input_area(Setup* pstp)
{
    int area_menu;
    do
    {
        printf("���L�������߂Ă�������\n");
        printf("1:�����i10�~10�j\n");
        printf("2:�����i15�~15�j\n");
        printf("3:�㋉�i20�~20�j\n");
        printf("4:�J�X�^��\n");
        printf("....");
        scanf("%s", tmp);
    }while(error_check(tmp, INT, 1, 4));
    area_menu = atoi(tmp);

    // �I�΂ꂽ��Փx�ɉ����čL����ݒ�
    if(area_menu==1)
    {
        pstp->row = 10;
        pstp->column = 10;
    }
    else if(area_menu==2)
    {
        pstp->row = 15;
        pstp->column = 15;
    }
    else if(area_menu==3)
    {
        pstp->row = 20;
        pstp->column = 20;
    }
    // �J�X�^��
    else
    {
        do
        {
            printf("�s������͂��Ă��������i5�`20�j....");
            scanf("%s", tmp);
        }while(error_check(tmp, INT, MIN_GRID, MAX_GRID) == 1);
        pstp->row = atoi(tmp);

        do{
            printf("�񐔂���͂��Ă��������i5�`20�j....");
            scanf("%s", tmp);
        }while(error_check(tmp, INT, MIN_GRID, MAX_GRID) == 1);
        pstp->column = atoi(tmp);
    }
    pstp->wrow = pstp->row + 2;
    pstp->wcolumn = pstp->column + 2;

    printf("\n");
}

// �n���̐������߂�
void input_bomb(Setup* pstp)
{
    int bomb_menu;
    do
    {
        printf("���n���������߂Ă��������@���i�j���͒n���̐�߂銄��\n");
        printf("1:�����i15���j\n");
        printf("2:�����i17.5���j\n");
        printf("3:�㋉�i20���j\n");
        printf("4:�J�X�^��\n");
        printf("....");
        scanf("%s", tmp);
    }while(error_check(tmp, INT, 1, 4) == 1);
    bomb_menu = atoi(tmp);

    // �I�΂ꂽ��Փx�ɉ����Ēn�������Z�o�Bpstp->bomb��int�Ȃ̂Ŏl�̌ܓ������
    if     (bomb_menu == 1) pstp->bomb = (pstp->row) * (pstp->column) * 0.15;
    else if(bomb_menu == 2) pstp->bomb = (pstp->row) * (pstp->column) * 0.175;
    else if(bomb_menu == 3) pstp->bomb = (pstp->row) * (pstp->column) * 0.20;
    else{
        do
        {
            printf("�n��������͂��Ă��������i1�`%d�j....", (pstp->row - 1) * (pstp->column - 1));
            scanf("%s", tmp);
        }while(error_check(tmp, INT, 1, (pstp->row - 1) * (pstp->column - 1)) == 1);
        pstp->bomb = atoi(tmp);
    }

    printf("\n");
}

// �����_���ȍ��W�Ɏw��̒n���𖄂߂�
void put_bomb(Setup* pstp, Grid** grd)
{
    int i = 1;
    int rand_X, rand_Y;
    while(i <= pstp->bomb)
    {
        rand_X = rand() % (pstp->row) + 1;
        rand_Y = rand() % (pstp->column) + 1;
        if(grd[rand_X][rand_Y].isbomb == 0)
        {
            grd[rand_X][rand_Y].isbomb = 1;
            i++;
        }
    }
}

// �Q�[����ʂ̕`��
void output_gameview(Setup* pstp, Grid** grd, Manager* pmng)
{
    int i, j;

    //1�s��
    printf("����");
    for(i = 1; i <= pstp->column; i++)
    {
        printf("%s", itoJapan(i));
    }
    printf("��");
    printf("\n");

    //2�s��
    for(i = 1; i <= pstp->column + 4; i++)
    {
        printf("��");
    }
    printf("\n");

    //3�s�ځ`������3�s��
    for(i = 1; i <= pstp->row; i++)
    {
        printf("%s��", itoJapan(i));
        for(j = 1; j <= pstp->column; j++)
        {
            // �Q�[���I�����A�n���}�X��񎦂���
            if(pmng->isfin != 0 && grd[i][j].isbomb == 1)
            {
                if(pmng->isfin == 1)      printf("��");
                else if(pmng->isfin == 2) printf("�~");
                else printf("error");
            }
            // �Q�[���I�����A�n���}�X�ȊO�͒ʏ펞�ƕς��Ȃ�
            else
            {
                if(grd[i][j].isopen == 1)
                {
                    strcpy(tmp, itoJapan(count_bomb(i, j, pstp, grd)));

                    if     (!strcmp(tmp, "�O"))            printf("��"); // ���͂ɉ����Ȃ��}�X
                    else if(!strcmp(tmp, "�X") || !strcmp(tmp, "error")) printf("error"); // �ُ�
                    else                                   printf("%s", tmp); // �����}�X
                }
                else if(grd[i][j].isflag == 1)             printf("��");
                else                                       printf("��"); // �J���Ă��Ȃ��}�X
            }
        }
        printf("��%s", itoJapan(i));
        printf("\n");
    }

    //������2�s��
    for(i = 1; i <= pstp->column + 4; i++){
        printf("��");
    }
    printf("\n");

    //������1�s��
    printf("����");
    for(i = 1; i <= pstp->column; i++){
        printf("%s", itoJapan(i));
    }
    printf("��");
    printf("\n");

    //�������̒n����
    printf("�������̒n�����F%d\n", pstp->bomb - pmng->sum_flag);
}

// �Q�[���I�����̉��
void output_fin(Manager* pmng)
{
    if(pmng->isfin == 1)
    {
        printf("�@�����@�@�@���@�@���@�@�@���@������\n");
        printf("���@�@�@�@���@���@�����@�����@��\n");
        printf("���@�����@�������@���@���@���@����\n");
        printf("���@�@���@���@���@���@�@�@���@��\n");
        printf("�@�����@�@���@���@���@�@�@���@������\n");
        printf("\n");
        printf("�@�����@���@�@�@�����@�@���@�@����\n");
        printf("���@�@�@���@�@�@���@�@���@���@���@��\n");
        printf("���@�@�@���@�@�@�����@�������@����\n");
        printf("���@�@�@���@�@�@���@�@���@���@���@��\n");
        printf("�@�����@�������@�����@���@���@���@��\n");
    }
    else if(pmng->isfin == 2)
    {
    	printf("�@�����@�@�@���@�@���@�@�@���@������\n");
    	printf("���@�@�@�@���@���@�����@�����@��\n");
    	printf("���@�����@�������@���@���@���@����\n");
    	printf("���@�@���@���@���@���@�@�@���@��\n");
    	printf("�@�����@�@���@���@���@�@�@���@������\n");
    	printf("\n");
    	printf("�@�����@�@���@�@�@���@�������@����\n");
    	printf("���@�@���@���@�@�@���@���@�@�@���@��\n");
    	printf("���@�@���@�@���@���@�@�����@�@����\n");
    	printf("���@�@���@�@���@���@�@���@�@�@���@��\n");
    	printf("�@�����@�@�@�@���@�@�@�������@���@��\n");
    }
    else return;

    do
    {
    printf("������x�V�т܂���?\n");
    printf("1:��Փx��ς��Ă�����x�V��\n");
    printf("2:��Փx��ς����ɂ�����x�V��\n");
    printf("3:�Q�[�����I���\n");
    printf("....");
    scanf("%s", tmp);
    }while(error_check(tmp, INT, 1, 3) == 1);
    pmng->isretry = atoi(tmp);

    if(pmng->isretry == 3) exit(0);
}

// �v���C���̍s��
void input_act(Setup* pstp, Grid** grd, Act* pact)
{
    while(1)
    {
        printf("�ǂ̃}�X�ɑ΂��čs�����邩���͂��Ă�������\n");
        do
        {
            printf("�ォ�琔���ĉ��s��?....");
            scanf("%s", tmp);
        }while(error_check(tmp, INT, 1, pstp->row) == 1);
        pact->select_X = atoi(tmp);

        do
        {
            printf("�����琔���ĉ����?....");
            scanf("%s", tmp);
        }while(error_check(tmp, INT, 1, pstp->column) == 1);
        pact->select_Y = atoi(tmp);

        do
        {
            printf("�s����I�����Ă�������\n");
            printf("1:�J��\n");
            printf("2:���𗧂Ă�E��菜��\n");
            printf("....");
            scanf("%s", tmp);
        }while(error_check(tmp, INT, 1, 2) == 1);
        pact->act_menu = atoi(tmp);
        
        if(pact->act_menu == 1 && grd[pact->select_X][pact->select_Y].isopen == 1)
            printf("�G���[�F���łɊJ���Ă���}�X���J�����Ƃ͂ł��܂���\n");
        else if(pact->act_menu == 1 && grd[pact->select_X][pact->select_Y].isflag == 1)
            printf("�G���[�F�����ݒu���ꂽ�}�X�͊J���܂���\n");
        else if(pact->act_menu == 2 && grd[pact->select_X][pact->select_Y].isopen == 1)
            printf("�G���[�F���łɊJ���Ă���}�X�Ɋ��͐ݒu�ł��܂���\n");
        else break;
    }
}

// �I�����ꂽ�}�X�Ƀv���C���̍s���𔽉f����
void update_grd(Setup* pstp, Grid** grd, Manager* pmng, Act* pact)
{
    // �J����
    if(pact->act_menu == 1) grd[pact->select_X][pact->select_Y].isopen = 1;
    // ��
    else if(pact->act_menu == 2)
    {
        // ���Ɋ�������Ȃ珜��
        if(grd[pact->select_X][pact->select_Y].isflag == 1)
        {
            grd[pact->select_X][pact->select_Y].isflag = 0;
            pmng->sum_flag--;
        }
        // �����Ȃ��Ȃ痧�Ă�
        else if(grd[pact->select_X][pact->select_Y].isflag == 0)
        {
            grd[pact->select_X][pact->select_Y].isflag = 1;
            pmng->sum_flag++;
        }
        else printf("error");
    }
    else printf("error");

    // ���͂̃}�X���J����
    rec_open(pstp, grd);
}

// �ċA�����Ŏ���̃}�X���J����
void rec_open(Setup* pstp, Grid** grd)
{
    int i, j , k, l, n = 0;

    // �}�X��������Ă���
    for(i = 1; i <= pstp->row; i++)
    {
        for(j = 1; j <= pstp->column; j++)
        {
            n = count_bomb(i, j, pstp, grd);
            if // ������ꏊ�ɂ���
            (
                n != 0 //���͂ɒn��������
                || grd[i][j].isopen != 1 // �J���Ă��Ȃ�
                || grd[i][j].isflag == 1 // ��������
                || grd[i][j].isbomb == 1 // �n��������
                || grd[i][j].ischeck == 1 // �T���ς�
            ) continue;

            grd[i][j].ischeck = 1;

            // ������ꏊ�̎��͂W�}�X(+���̏ꏊ)�𒲂ׂ�
            for(k = i - 1; k <= i + 1; k++)
            {
                for(l = j - 1; l <= j + 1; l++)
                {
                    if(
                        grd[k][l].isopen == 0
                        && grd[k][l].isflag == 0
                    ) grd[k][l].isopen = 1; 
                }
            }
            // �Ֆʂ��ω������̂ōċA���ĒT��
            rec_open(pstp, grd);
        }
    }
}

// �Q�[���̏I�������𖞂����Ă��邩
void game_fin(Setup* pstp, Grid** grd, Manager* pmng, Act* pact)
{
    int i, j;
    int sum_open = 0;
    // �J���Ă���}�X���𐔂���
    for(i = 1; i <= pstp->row; i++)
    {
        for(j = 1; j <= pstp->column; j++)
        {
            if(grd[i][j].isopen == 1) sum_open++;
        }
    }

    // �I�������ɏƂ炵���킹��
    if
    (
        grd[pact->select_X][pact->select_Y].isopen == 1
        && grd[pact->select_X][pact->select_Y].isbomb == 1
    ) pmng->isfin = 2;
    else if(sum_open == (pstp->row) * (pstp->column) - pstp->bomb) pmng->isfin = 1;
}

// ���p������S�p�ɕς���
char* itoJapan(int num)
{
    num = num % 10;

    if     (num == 0) strcpy(tmp, "�O");
    else if(num == 1) strcpy(tmp, "�P");
    else if(num == 2) strcpy(tmp, "�Q");
    else if(num == 3) strcpy(tmp, "�R");
    else if(num == 4) strcpy(tmp, "�S");
    else if(num == 5) strcpy(tmp, "�T");
    else if(num == 6) strcpy(tmp, "�U");
    else if(num == 7) strcpy(tmp, "�V");
    else if(num == 8) strcpy(tmp, "�W");
    else if(num == 9) strcpy(tmp, "�X");
    else strcpy(tmp, "error");

    return tmp;
}

// �w�肳�ꂽ�}�X�̎��͂̒n���𐔂���
int count_bomb(int center_X, int center_Y, Setup* pstp, Grid** grd)
{
    int i, j;
    int sum_bomb = 0;

    for(i = center_X - 1; i <= center_X + 1; i++)
    {
        for(j = center_Y - 1; j <= center_Y + 1; j++)
        {
            if
            (
                i >= 1
                && i <= pstp->row
                && j >= 1
                && j <= pstp->column
                && grd[i][j].isbomb == 1
            ) sum_bomb++;
        }
    }
    return sum_bomb;
}

// ���͂��ꂽ�����񂪗L���Ȑ��l�ł��邩�`�F�b�N
int error_check(char tmp[], int tmp_type, float min_value, float max_value)
{
    float check_value;
    // ���l���ǂ���
    if(str_check(tmp, tmp_type) == 0) check_value = atof(tmp);
    else
    {
        printf("�G���[�F���l����͂��Ă�������\n");
        printf("\n");
        return 1;
    }

    // �͈͓����ǂ���
    if(check_value <= max_value && check_value >= min_value)
    {
        return 0;
    }
    else
    {
        printf("�G���[�F���͒l���͈͊O�ł�\n");
        if(tmp_type == INT) printf("�L���͈́F%.0f <= ���� <= %.0f", min_value, max_value);
        else if(tmp_type == FLOAT) printf("�L���͈́F%f <= ���� <= %f", min_value, max_value);
        else printf("error\n");
        printf("\n");
        return 1;
    }
}

// ���͂��ꂽ�����񂪐��l�ł��邩�ǂ������`�F�b�N
int str_check(char str[], int str_type)
{
    int i;
    for(i = 0; i < strlen(str); i++)
    {
        if
        (
            str[i] == '-' // �}�C�i�X����
            || (str[i] >= '0'&& str[i] <= '9') // �O�`�X�̐���
            || (str[i] = '.' && str_type == FLOAT) // �����_����float�^
        ) continue;
        else return 1;
    }
    return 0;
}

// �^�C�g����ʂ̕`��
void title_scene(Manager* pmng)
{
    while(1)
    {
        printf("\n");
    	printf("���@�@�@���@�������@���@�@�@���@������\n");
    	printf("�����@�����@�@���@�@�����@�@���@��\n");
    	printf("���@���@���@�@���@�@���@���@���@����\n");
    	printf("���@�@�@���@�@���@�@���@�@�����@��\n");
    	printf("���@�@�@���@�������@���@�@�@���@�������@Ver.2\n");
    	printf("����������������������������������������������������\n");
    	printf("�����@���@���@���@�����@�����@�����@�@�����@����\n");
    	printf("���@�@���@���@���@���@�@���@�@���@���@���@�@���@��\n");
    	printf("�����@���@���@���@�����@�����@�����@�@�����@����\n");
    	printf("�@���@���@���@���@���@�@���@�@���@�@�@���@�@���@��\n");
    	printf("�����@�@���@���@�@�����@�����@���@�@�@�����@���@��\n");
    	printf("����������������������������������������������������������\n");
    	printf("\n");
        printf("�@�@�@�@�@�@    [�@����@]\n");
        printf("    �����L�[:���l���́@Enter�L�[:�m��\n");
        printf("\n");
        printf("    �@�@0�@���@Enter �Ł@�͂��߂�\n");
        printf("    �@�@�@�@�@�@....");
        scanf("%s", tmp);
        if(!strcmp(tmp, "0")) break;
        if(!strcmp(tmp, "d"))
        {
            pmng->isdebug = 1;
            break;
        }
        system("cls");
    }
    system("cls");
}

// �f�o�b�O���[�h
void debug(Setup* pstp, Grid** grd, Manager* pmng)
{
    if(pmng->isdebug != 1) return;
    int i, j;
    printf("\n\n==debug==\n");
    printf("%d\n", pstp->row);
    printf("%d\n", pstp->column);
    printf("%d\n", pstp->bomb);
    for(i = 0; i < pstp->wrow; i++)
    {
        for(j = 0; j < pstp->wcolumn; j++)
        {
            printf("%d ", grd[i][j].isopen);
        }
        printf("\n");
    }
    printf("\n");
    for(i = 0; i < pstp->wrow; i++)
    {
        for(j = 0; j < pstp->wcolumn; j++)
        {
            printf("%d ", grd[i][j].isbomb);
        }
        printf("\n");
    }
    printf("\n==debug==\n\n");
}

// ��ʃN���A
void cls(int isdebug)
{
    if(isdebug == 1) return;
    system("cls");
}