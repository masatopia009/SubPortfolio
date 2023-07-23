//コマンドプロンプトで動作可能

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#define MAX_GRID 20 // 最大マス数
#define MIN_GRID 5 // 最小マス数
#define INT 0 // int型を表すフラグ
#define FLOAT 1 // float型を表すフラグ

typedef struct
{
    int row;//壁を含まない
    int column;//壁を含まない
    int wrow;//壁を含む
    int wcolumn;//壁を含む
    int bomb;//設定した地雷数
}Setup;//盤面の初期設定

typedef struct
{
    int isopen; // マスの開閉フラグ
    int isflag; // 旗が立てられているかフラグ
    int isbomb; // 地雷フラグ
    int ischeck; // 探索フラグ
}Grid;//各マスが持つフラグ

typedef struct
{
    int isdebug; // デバッグモードか
    int isfin; // ゲームが終了したか（０：デフォルト　１：ゲームクリア　２：ゲームオーバー）
    int isretry; // リトライするか（０：デフォルト　１：難易度を変えてリトライ　２：難易度を変えずにリトライ　３：終了）
    int sum_flag;//全体の旗の数
}Manager;//ゲームの進行状況とゲーム全体のカウンター

typedef struct
{
    int act_menu; // 行動選択（１：開ける　２：旗）
    int select_X; // 選んだx座標
    int select_Y; // 選んだy座標
}Act;

// プロトタイプ宣言
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

    // ゲーム全体のループ
    while(1)
    {
        mng.isfin = 0;
        mng.sum_flag = 0;
        srand(time(NULL));

        if(mng.isretry != 2) grd = setup_grd(&stp);
        mng.isretry = 0;
        init_grd(&stp, grd);

        cls(mng.isdebug);

        // １ターンのループ
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

// ２次元配列を作成
Grid** setup_grd(Setup* pstp)
{
    Grid** grd;
    Grid* base_grd;
    int i, j;

    input_area(pstp);
    input_bomb(pstp);

    grd = (Grid**)malloc(sizeof(Grid*) * pstp->wrow);
    base_grd = (Grid*)malloc(sizeof(Grid) * pstp->wrow * pstp->wcolumn);
    //壁を含めたサイズ
    for(i = 0; i < pstp->wrow; i++)
    {
        grd[i] = base_grd + i * pstp->wcolumn;
        //1列目の各要素を各行の先頭とみなして先頭アドレスを代入している
    }
    return grd;
}

void init_grd(Setup* pstp, Grid** grd)
{
    int i, j;
    // 各マスのフラグを初期化
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
            //壁は2, そうでないマスは0で初期化

            grd[i][j].isflag = 0;
            grd[i][j].isbomb = 0;
            grd[i][j].ischeck = 0;
        }
    }
    put_bomb(pstp, grd);
}

// マスの広さを決める
void input_area(Setup* pstp)
{
    int area_menu;
    do
    {
        printf("■広さを決めてください\n");
        printf("1:初級（10×10）\n");
        printf("2:中級（15×15）\n");
        printf("3:上級（20×20）\n");
        printf("4:カスタム\n");
        printf("....");
        scanf("%s", tmp);
    }while(error_check(tmp, INT, 1, 4));
    area_menu = atoi(tmp);

    // 選ばれた難易度に応じて広さを設定
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
    // カスタム
    else
    {
        do
        {
            printf("行数を入力してください（5～20）....");
            scanf("%s", tmp);
        }while(error_check(tmp, INT, MIN_GRID, MAX_GRID) == 1);
        pstp->row = atoi(tmp);

        do{
            printf("列数を入力してください（5～20）....");
            scanf("%s", tmp);
        }while(error_check(tmp, INT, MIN_GRID, MAX_GRID) == 1);
        pstp->column = atoi(tmp);
    }
    pstp->wrow = pstp->row + 2;
    pstp->wcolumn = pstp->column + 2;

    printf("\n");
}

// 地雷の数を決める
void input_bomb(Setup* pstp)
{
    int bomb_menu;
    do
    {
        printf("■地雷数を決めてください　※（）内は地雷の占める割合\n");
        printf("1:初級（15％）\n");
        printf("2:中級（17.5％）\n");
        printf("3:上級（20％）\n");
        printf("4:カスタム\n");
        printf("....");
        scanf("%s", tmp);
    }while(error_check(tmp, INT, 1, 4) == 1);
    bomb_menu = atoi(tmp);

    // 選ばれた難易度に応じて地雷数を算出。pstp->bombはintなので四捨五入される
    if     (bomb_menu == 1) pstp->bomb = (pstp->row) * (pstp->column) * 0.15;
    else if(bomb_menu == 2) pstp->bomb = (pstp->row) * (pstp->column) * 0.175;
    else if(bomb_menu == 3) pstp->bomb = (pstp->row) * (pstp->column) * 0.20;
    else{
        do
        {
            printf("地雷数を入力してください（1～%d）....", (pstp->row - 1) * (pstp->column - 1));
            scanf("%s", tmp);
        }while(error_check(tmp, INT, 1, (pstp->row - 1) * (pstp->column - 1)) == 1);
        pstp->bomb = atoi(tmp);
    }

    printf("\n");
}

// ランダムな座標に指定個の地雷を埋める
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

// ゲーム画面の描画
void output_gameview(Setup* pstp, Grid** grd, Manager* pmng)
{
    int i, j;

    //1行目
    printf("★回");
    for(i = 1; i <= pstp->column; i++)
    {
        printf("%s", itoJapan(i));
    }
    printf("回★");
    printf("\n");

    //2行目
    for(i = 1; i <= pstp->column + 4; i++)
    {
        printf("回");
    }
    printf("\n");

    //3行目～下から3行目
    for(i = 1; i <= pstp->row; i++)
    {
        printf("%s回", itoJapan(i));
        for(j = 1; j <= pstp->column; j++)
        {
            // ゲーム終了時、地雷マスを提示する
            if(pmng->isfin != 0 && grd[i][j].isbomb == 1)
            {
                if(pmng->isfin == 1)      printf("◎");
                else if(pmng->isfin == 2) printf("×");
                else printf("error");
            }
            // ゲーム終了時、地雷マス以外は通常時と変わらない
            else
            {
                if(grd[i][j].isopen == 1)
                {
                    strcpy(tmp, itoJapan(count_bomb(i, j, pstp, grd)));

                    if     (!strcmp(tmp, "０"))            printf("□"); // 周囲に何もないマス
                    else if(!strcmp(tmp, "９") || !strcmp(tmp, "error")) printf("error"); // 異常
                    else                                   printf("%s", tmp); // 数字マス
                }
                else if(grd[i][j].isflag == 1)             printf("▲");
                else                                       printf("■"); // 開けていないマス
            }
        }
        printf("回%s", itoJapan(i));
        printf("\n");
    }

    //下から2行目
    for(i = 1; i <= pstp->column + 4; i++){
        printf("回");
    }
    printf("\n");

    //下から1行目
    printf("★回");
    for(i = 1; i <= pstp->column; i++){
        printf("%s", itoJapan(i));
    }
    printf("回★");
    printf("\n");

    //未発見の地雷数
    printf("未発見の地雷数：%d\n", pstp->bomb - pmng->sum_flag);
}

// ゲーム終了時の画面
void output_fin(Manager* pmng)
{
    if(pmng->isfin == 1)
    {
        printf("　■■　　　■　　■　　　■　■■■\n");
        printf("■　　　　■　■　■■　■■　■\n");
        printf("■　■■　■■■　■　■　■　■■\n");
        printf("■　　■　■　■　■　　　■　■\n");
        printf("　■■　　■　■　■　　　■　■■■\n");
        printf("\n");
        printf("　■■　■　　　■■　　■　　■■\n");
        printf("■　　　■　　　■　　■　■　■　■\n");
        printf("■　　　■　　　■■　■■■　■■\n");
        printf("■　　　■　　　■　　■　■　■　■\n");
        printf("　■■　■■■　■■　■　■　■　■\n");
    }
    else if(pmng->isfin == 2)
    {
    	printf("　□□　　　□　　□　　　□　□□□\n");
    	printf("□　　　　□　□　□□　□□　□\n");
    	printf("□　□□　□□□　□　□　□　□□\n");
    	printf("□　　□　□　□　□　　　□　□\n");
    	printf("　□□　　□　□　□　　　□　□□□\n");
    	printf("\n");
    	printf("　□□　　□　　　□　□□□　□□\n");
    	printf("□　　□　□　　　□　□　　　□　□\n");
    	printf("□　　□　　□　□　　□□　　□□\n");
    	printf("□　　□　　□　□　　□　　　□　□\n");
    	printf("　□□　　　　□　　　□□□　□　□\n");
    }
    else return;

    do
    {
    printf("もう一度遊びますか?\n");
    printf("1:難易度を変えてもう一度遊ぶ\n");
    printf("2:難易度を変えずにもう一度遊ぶ\n");
    printf("3:ゲームを終わる\n");
    printf("....");
    scanf("%s", tmp);
    }while(error_check(tmp, INT, 1, 3) == 1);
    pmng->isretry = atoi(tmp);

    if(pmng->isretry == 3) exit(0);
}

// プレイヤの行動
void input_act(Setup* pstp, Grid** grd, Act* pact)
{
    while(1)
    {
        printf("どのマスに対して行動するか入力してください\n");
        do
        {
            printf("上から数えて何行目?....");
            scanf("%s", tmp);
        }while(error_check(tmp, INT, 1, pstp->row) == 1);
        pact->select_X = atoi(tmp);

        do
        {
            printf("左から数えて何列目?....");
            scanf("%s", tmp);
        }while(error_check(tmp, INT, 1, pstp->column) == 1);
        pact->select_Y = atoi(tmp);

        do
        {
            printf("行動を選択してください\n");
            printf("1:開く\n");
            printf("2:旗を立てる・取り除く\n");
            printf("....");
            scanf("%s", tmp);
        }while(error_check(tmp, INT, 1, 2) == 1);
        pact->act_menu = atoi(tmp);
        
        if(pact->act_menu == 1 && grd[pact->select_X][pact->select_Y].isopen == 1)
            printf("エラー：すでに開いているマスを開くことはできません\n");
        else if(pact->act_menu == 1 && grd[pact->select_X][pact->select_Y].isflag == 1)
            printf("エラー：旗が設置されたマスは開けません\n");
        else if(pact->act_menu == 2 && grd[pact->select_X][pact->select_Y].isopen == 1)
            printf("エラー：すでに開いているマスに旗は設置できません\n");
        else break;
    }
}

// 選択されたマスにプレイヤの行動を反映する
void update_grd(Setup* pstp, Grid** grd, Manager* pmng, Act* pact)
{
    // 開ける
    if(pact->act_menu == 1) grd[pact->select_X][pact->select_Y].isopen = 1;
    // 旗
    else if(pact->act_menu == 2)
    {
        // 既に旗があるなら除去
        if(grd[pact->select_X][pact->select_Y].isflag == 1)
        {
            grd[pact->select_X][pact->select_Y].isflag = 0;
            pmng->sum_flag--;
        }
        // 旗がないなら立てる
        else if(grd[pact->select_X][pact->select_Y].isflag == 0)
        {
            grd[pact->select_X][pact->select_Y].isflag = 1;
            pmng->sum_flag++;
        }
        else printf("error");
    }
    else printf("error");

    // 周囲のマスも開ける
    rec_open(pstp, grd);
}

// 再帰処理で周りのマスも開ける
void rec_open(Setup* pstp, Grid** grd)
{
    int i, j , k, l, n = 0;

    // マスを一つずつ見ていく
    for(i = 1; i <= pstp->row; i++)
    {
        for(j = 1; j <= pstp->column; j++)
        {
            n = count_bomb(i, j, pstp, grd);
            if // 今居る場所について
            (
                n != 0 //周囲に地雷がある
                || grd[i][j].isopen != 1 // 開いていない
                || grd[i][j].isflag == 1 // 旗がある
                || grd[i][j].isbomb == 1 // 地雷がある
                || grd[i][j].ischeck == 1 // 探索済み
            ) continue;

            grd[i][j].ischeck = 1;

            // 今居る場所の周囲８マス(+この場所)を調べる
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
            // 盤面が変化したので再帰して探索
            rec_open(pstp, grd);
        }
    }
}

// ゲームの終了条件を満たしているか
void game_fin(Setup* pstp, Grid** grd, Manager* pmng, Act* pact)
{
    int i, j;
    int sum_open = 0;
    // 開いているマス数を数える
    for(i = 1; i <= pstp->row; i++)
    {
        for(j = 1; j <= pstp->column; j++)
        {
            if(grd[i][j].isopen == 1) sum_open++;
        }
    }

    // 終了条件に照らし合わせる
    if
    (
        grd[pact->select_X][pact->select_Y].isopen == 1
        && grd[pact->select_X][pact->select_Y].isbomb == 1
    ) pmng->isfin = 2;
    else if(sum_open == (pstp->row) * (pstp->column) - pstp->bomb) pmng->isfin = 1;
}

// 半角数字を全角に変える
char* itoJapan(int num)
{
    num = num % 10;

    if     (num == 0) strcpy(tmp, "０");
    else if(num == 1) strcpy(tmp, "１");
    else if(num == 2) strcpy(tmp, "２");
    else if(num == 3) strcpy(tmp, "３");
    else if(num == 4) strcpy(tmp, "４");
    else if(num == 5) strcpy(tmp, "５");
    else if(num == 6) strcpy(tmp, "６");
    else if(num == 7) strcpy(tmp, "７");
    else if(num == 8) strcpy(tmp, "８");
    else if(num == 9) strcpy(tmp, "９");
    else strcpy(tmp, "error");

    return tmp;
}

// 指定されたマスの周囲の地雷を数える
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

// 入力された文字列が有効な数値であるかチェック
int error_check(char tmp[], int tmp_type, float min_value, float max_value)
{
    float check_value;
    // 数値かどうか
    if(str_check(tmp, tmp_type) == 0) check_value = atof(tmp);
    else
    {
        printf("エラー：数値を入力してください\n");
        printf("\n");
        return 1;
    }

    // 範囲内かどうか
    if(check_value <= max_value && check_value >= min_value)
    {
        return 0;
    }
    else
    {
        printf("エラー：入力値が範囲外です\n");
        if(tmp_type == INT) printf("有効範囲：%.0f <= 入力 <= %.0f", min_value, max_value);
        else if(tmp_type == FLOAT) printf("有効範囲：%f <= 入力 <= %f", min_value, max_value);
        else printf("error\n");
        printf("\n");
        return 1;
    }
}

// 入力された文字列が数値であるかどうかをチェック
int str_check(char str[], int str_type)
{
    int i;
    for(i = 0; i < strlen(str); i++)
    {
        if
        (
            str[i] == '-' // マイナス符号
            || (str[i] >= '0'&& str[i] <= '9') // ０～９の数字
            || (str[i] = '.' && str_type == FLOAT) // 小数点かつfloat型
        ) continue;
        else return 1;
    }
    return 0;
}

// タイトル画面の描画
void title_scene(Manager* pmng)
{
    while(1)
    {
        printf("\n");
    	printf("■　　　■　□□□　■　　　■　□□□\n");
    	printf("■■　■■　　□　　■■　　■　□\n");
    	printf("■　■　■　　□　　■　■　■　□□\n");
    	printf("■　　　■　　□　　■　　■■　□\n");
    	printf("■　　　■　□□□　■　　　■　□□□　Ver.2\n");
    	printf("＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝\n");
    	printf("□□　■　■　■　□□　■■　□□　　■■　□□\n");
    	printf("□　　■　■　■　□　　■　　□　□　■　　□　□\n");
    	printf("□□　■　■　■　□□　■■　□□　　■■　□□\n");
    	printf("　□　■　■　■　□　　■　　□　　　■　　□　□\n");
    	printf("□□　　■　■　　□□　■■　□　　　■■　□　□\n");
    	printf("＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝\n");
    	printf("\n");
        printf("　　　　　　    [　操作　]\n");
        printf("    数字キー:数値入力　Enterキー:確定\n");
        printf("\n");
        printf("    　　0　→　Enter で　はじめる\n");
        printf("    　　　　　　....");
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

// デバッグモード
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

// 画面クリア
void cls(int isdebug)
{
    if(isdebug == 1) return;
    system("cls");
}