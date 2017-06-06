#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define M 54 //カード総枚数
#define N 4  //カード模様の種類数
#define L 20 //文字列の長さ
#define P 10 //プレーヤーの最大人数

#define FALSE 0
#define TRUE  1

// カードの定義
typedef struct card{
    int  number;   //数字
    char ch;       //文字（A,J,Q,K 用)
    char mark[L];  //模様
}CARD;

// プレーヤーが所持するカードの定義
typedef struct handCard{
    int num; 							 // 数字
    struct handCard *link; // 連結リスト用ポインター
}handCard;

// プレーヤーの定義
typedef struct player{
    int  ID;         //ID
    char name[L];    //名前
    int  count;      //手元にあるカードの枚数
    int  rank;       //勝敗の順位
    handCard *front; //手元にあるカードのリストへ指す先頭ポインター
}PLAYER;

// プレーヤー全員を初期化する
void initPlayer(PLAYER x[P], int n, int m){
    int i = 0, j = 0;
    
    while(i < (n+m)){
        x[i].ID = i+1;
        
        if(i < n){
            printf("No.%02d Name : ", x[i].ID);
            scanf("%s", x[i].name);
        } else {
            j ++;
            printf("No.%02d Name = COM%02d \n", x[i].ID, j);
            sprintf(x[i].name, "COM%02d", j);
        }
        
        x[i].count = 0;
        x[i].rank  = n+m;
        x[i].front = NULL;
        
        i++;
    }
    
    putchar('\n');
}

// 全部のカードを初期化する
void initCard(CARD a[M]){
    int i = 0, j, k;
    char mark[6][L]={"Heart", "Diamond", "Club", "Spade", "joker", "JOKER"};
    
    while(i < 52){
        j = i % 13;
        a[i].number = j + 1;
        
        switch(a[i].number){
            case 1:  a[i].ch = 'A';
                break;
            case 11: a[i].ch = 'J';
                break;
            case 12: a[i].ch = 'Q';
                break;
            case 13: a[i].ch = 'K';
                break;
            default: a[i].ch = ' ';
        }
        
        k = i / 13;
        strcpy(a[i].mark, mark[k]);
        
        i++;
    }
    
    a[52].number = 100;	a[52].ch='j'; strcpy(a[52].mark, mark[4]);
    a[53].number = 200; a[53].ch='j'; strcpy(a[53].mark, mark[5]);
}

// プレーヤー全員を表示する
void listPlayer(PLAYER x[P], int n){
    int i;
    
    puts("-----------------------------");
    printf("%s\t%s\t%s\t%s\n", "No.", "Name", "count", "rank");
    for(i = 0; i < n; i++)
        printf("%d\t%s\t%d\t%d\n", x[i].ID, x[i].name, x[i].count, x[i].rank);
    puts("-----------------------------");
    
    putchar('\n');
}

// 全部のカードを表示する
void listCard(CARD a[M], int n){
    int i;
    printf("%s\t%s\t%s\t%s\n","No.","CardNo.","Char","Mark");
    
    for(i = 0;i < n; i++)
        printf("%d\t%d\t%c\t%s\n",i+1,a[i].number,a[i].ch, a[i].mark);
}

// カードをシャッフルする
void shuffle(CARD a[M], int n){
    CARD temp;
    int i, j;
    
    for(i = 1; i < n; i++){ // i>0
        j = rand() % i;
        
        if(i != j){
            temp = a[i];
            a[i] = a[j];
            a[j] = temp;
        }
    }
    
    puts("シャッフル完了.");
}

// カードを各プレーヤーに時計順に配る
// 配るときに、手元に同じ数字を持つカードを抜く
// 新しいカードを連結リストの先頭に挿入する
void dealCards(PLAYER x[P], CARD a[M], int n){
    handCard *hc, *p1 = NULL, *p2, *p3;
    int i = 0, j = 0, flag;
    
    while(i < M-1){
        // 53枚のカードを順に各プレーヤーに配る
        // x[j]リストに a[i]番のカードと同じ数字をもつカードがあるかを調べる
        // flag 1:なし 0:ある
        // p1 がその見つかったカードに指す
        flag = 1;
        
        if(x[j].front != NULL){
            for(p1 = x[j].front; p1 != NULL; p1 = p1->link){
                if(p1->num == a[i].number){
                    flag = 0;
                    break;
                }
            }
        }
        
        if(flag == 0){ // p1が指しているカードを抜く
            if(p1->num == x[j].front->num){
                // 先頭
                x[j].front = x[j].front->link;
                p1->link = NULL;
            }else{
                // 2番目から最後尾まで
                p2 = x[j].front;
                p3 = p2->link;
                
                while(p2 ->link != NULL){
                    if(p1->num == p3->num ){
                        p2->link = p1->link;
                        p1->link = NULL;
                        break;
                    }
                    p2 = p3;
                    p3 = p3->link;
                }
            }
            
            x[j].count--;
            free(p1);
        }else if(flag == 1 || x[j].front == NULL){
            // 手札を確保し, カードを1枚取得する
            hc = (handCard *) malloc (sizeof(handCard));
            hc->num = a[i].number;
            hc->link = x[j].front; // プレーヤーリストとつなげる
            
            // 各プレーヤーが所持するカードリストに新しい札をリストの先頭に入れる
            x[j].front = hc;
            x[j].count ++;
            
            j = (j+1) % n; // 順にプレーヤーを変える(リング）
        }
        
        i ++; // 順にカードを変える
    }
    
    puts("カードをプレイヤーに配布しました.");
    putchar('\n');
}

//　プレーヤーが所持するカードリストを表示
void listHC(handCard *hc, int fill){
    handCard *p;
    
    for(p = hc; p != NULL; p = p->link){
        if(fill == FALSE)
            printf(" %d ", p->num);
        else
            printf(" * ");
    }
    
    printf("\n");
}

// 各プレーヤーが所持するカードリストを表示
void listHandCard(PLAYER x[P], int n){
    int i;
    
    for(i = 0; i < n; i++){
        printf("player No.%d count: %d -> ",x[i].ID, x[i].count);
        listHC(x[i].front, FALSE);
    }
}

// ババ抜き
void babaNuki(PLAYER x[P], int n, int com){
    handCard *p1, *p2, *p3 = NULL;
    int i = 1, j, k, winner = 0, m = 0, flag;
    int member = n+com;
    
    printf("\nゲーム開始.");
    printf("5 ");
    usleep(1000000);
    printf("4 ");
    usleep(1000000);
    printf("3 ");
    usleep(1000000);
    printf("2 ");
    usleep(1000000);
    printf("1 ");
    usleep(1000000);
    putchar('\n');
    while(getchar() != '\n');
    
    
    while(winner < member-1){ // ゲーム終了条件：プレーヤーが残り 1 人のみ
        listPlayer(x, member);
        putchar('\n');
        
        /* 2つのリストを指定する */
        while(x[i].count == 0){
            i = (i + 1) % member;
        }
        
        j = (i + 1) % member;
        while (x[j].count == 0){
            j = (j + 1) % member;
        }
        
        if(x[i].ID > n) puts("Computer が 操作しています.");
        printf("-------- %s Turn (%s -> %s) --------\n", x[i].name, x[i].name, x[j].name);
        /* 手札表示 */
        // 自分
        p1 = x[i].front;
        printf("あなたの手札 [%2d枚] :", x[i].count);
        listHC(x[i].front, FALSE);
        putchar('\n');
        
        // 相手
        p2 = x[j].front;
        printf("相手  の手札 [%2d枚] :", x[j].count);
        listHC(x[j].front, TRUE);
        putchar('\n');
        
        /* カード操作 */
        // 相手(x[j])のカードを抜く
        // ポインターp2 は抜いたカードに指す
        // リストを調整する
        if(x[i].ID <= n){
            printf("> 抜き取るカードの位置を選択 (1 -- %d) : ", x[j].count);
            scanf("%d", &m);
        } else{
            m = (rand() % x[j].count) + 1;
            printf("> %sは %d 番目の位置を選択しました.", x[i].name, m);
        }
        putchar('\n');
        
        if(m == 1){ // 先頭
            x[j].front = x[j].front->link;
        } else if(m == x[j].count){ // 最後尾
            for( ; p2->link !=NULL; p2 = p2->link){
                p3 = p2;
            }
            
            p3->link = NULL;
        } else{ // 真中
            for(k = 1; k < m; k++, p2 = p2->link){
                p3 = p2;
            }
            
            p3->link = p2->link;
        }
        
        p2->link = NULL;
        x[j].count --;
        
        if(x[j].count == 0){
            winner ++;
            x[j].rank = winner;
            printf("\n> %s さんがあがりました. %d位です.\n\n", x[j].name, x[j].rank);
        }
        
        if(p2->num != 100)
            printf("> 抜き取ったカードは %d でした.\n", p2->num);
        else {
            puts("> 残念!");
            puts("> 抜き取ったカードは joker でした.");
        }
        
        /* ババ抜き */
        // 自分(x[i])のリストにある p1 カードを p2 が指しているカードと比べる
        // 同じ数字をもつカードは見つかったらそのカードを抜く
        // 見つからなかったら, p2 が指しているカードを自分のリストに入れる
        // 対応するリストを調整する
        flag = 1; //1:なし 0:ある
        
        if(p1->num == p2->num){ //先頭
            x[i].front = x[i].front->link;
            p1->link = NULL;
            flag = 0;
        } else {       			//2 番目から最後尾まで
            p3 = p1;
            p1 = p1->link;
            
            while(p3->link != NULL){
                if(p1->num == p2->num){
                    p3->link = p1->link;
                    flag = 0;
                    break;
                }
                
                p3 = p1;
                p1 = p1->link;
            }
        }
        
        if(flag == 0){
            x[i].count --;
            
            if(x[i].count == 0){
                winner++;
                x[i].rank = winner;
                printf("\n> %s さんがあがりました! %d位です.\n\n", x[i].name, x[i].rank);
            }
            
            free(p1);
            free(p2);
        } else if(flag == 1) {
            // 同じカードはなかった場合,
            // 便利のため, p2 が指しているカードを x[i]リストの先頭に入れる
            p2->link = x[i].front;
            x[i].front = p2;
            x[i].count++;
        }
        
        if(x[i].count != 0){
            printf("> あなたの手札 [%2d枚] :", x[i].count);
            listHC(x[i].front, FALSE);
        }
        // if(x[j].count != 0){
        // printf("> 相手  の手札 [%2d枚] :", x[j].count);
        // listHC(x[j].front, FALSE);
        // }
        
        /* 待機状態 */
        if(winner < member-1){
            if(x[i].ID <= n)
                while(getchar() != '\n');
            printf("5 ");
            usleep(1000000);
            printf("4 ");
            usleep(1000000);
            printf("3 ");
            usleep(1000000);
            printf("2 ");
            usleep(1000000);
            printf("1 ");
            usleep(1000000);
            printf("\n\n\n\n\n\n\n\n\n");
        }
        
        i = (i + 1) % member;
    }
    
    /* 待機状態 */
    while(getchar() != '\n');
    puts("---------------------------------------------");
    printf("\nEnterを入力して最終結果を出力.");
    putchar('\n');
    while(getchar() != '\n');
}

int menu(){
    int n = 5;
    while (4 < n) {
        
        puts("----------MENU----------");
        printf("1:ババ抜きをする\n");
        printf("2:ババ抜き時間短縮する(じゃんけん)\n");
        printf("3:おわる\n");
        puts("------------------------");
        printf(">> ");
        scanf("%d", &n);
    }
    return n;
}

int setPlayerHand();
int setComputerHand();
int judge(int playerHand, int computerHand);

int setPlayerHand(){
    int playerHand = 0;
    printf("\nグー:0  チョキ:1  パー:2\nあなたの手を選択してください : ");
    scanf("%d", &playerHand);
    for (;playerHand != 0 && playerHand != 1 && playerHand != 2; ) {
        printf("正しくない数値です \nもう1度選びなおしてください : ");
        scanf("%d", &playerHand);
    }
    return playerHand;
}

int setComputerHand(){
    int computerHand = 0;
    srand((unsigned int)time(NULL));
    computerHand = rand()%3;
    return computerHand;
}

int judge(int playerHnad, int computerHand){
    int judgement = 0;
    if(playerHnad == computerHand)
        judgement = 5;
    else if(computerHand == (playerHnad + 1) % 3)
        judgement = 3;
    else
        judgement = 4;
    return judgement;
}


int main(){
    CARD playCard[M];
    PLAYER player[P];
    int n = 1, m = 0;
    srand((unsigned)time(NULL));
    
    while ((n = 1)) {
        switch (menu()) {
            case 1:
                initCard(playCard);
                
                if(n != 10){
                    printf("コンピューターの人数を入力(0 -- %2d) : ", P-n);
                    scanf("%d", &m);
                }
                putchar('\n');
                
                /* プレイヤーの初期化と情報表示 */
                initPlayer(player, n, m);
                listPlayer(player, n+m);
                
                /* カードをシャッフルし, プレイヤーに配布 */
                shuffle(playCard, M-1);
                dealCards(player, playCard, n+m);
                // listHandCard(player, n);
                
                /* ババ抜き */
                babaNuki(player, n, m);
                listPlayer(player, n+m);
                break;
            case 2:
                switch(judge(setPlayerHand(), setComputerHand())){
                    case 3:
                        printf("あなたの勝ちです\n");
                        break;
                    case 4:
                        printf("あなたの負けです\n");
                        break;
                    case 5:
                        printf("引き分けです\n");
                        break;
                }
            case 3:
                return 0;
        }
    }
}
