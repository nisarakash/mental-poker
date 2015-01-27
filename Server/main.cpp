#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>
#include <iostream>
#include <arpa/inet.h>

using namespace std;

int gcd(int x,int y);
int power_mod(int number,int key, int n);
int encryption(int number,int key, int n);
int decryption(int number,int key, int n);
int modInverse(int a, int m);
void randomize ( int arr[], int n );
void swap (int *a, int *b);
int generateKey(int p);

int cards[52];
string suit[] = {"Hearts", "Club", "Diamond", "Spades"};
string facevalue[] = {"Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine", "Ten", "Jack", "Queen", "King", "Ace"};
vector<string> deck;
;
int main(int argc, char**argv)
{
    int money[1] = {1000},bettingMoney[1],mycards[5],cheating[1],win[1];
    int opponentMoney[1],opponentKey[1],opponentcards[5],opponentDecrycards[5];
    int prime[1],key[1],inverseKey[1];
    int sockfd,n;
    struct sockaddr_in servaddr,cliaddr;
    socklen_t len;
    int i=0,j=0;

    for(i=0;i<4;i++)
    {
        for(j=0;j<13;j++)
        {
            string s;
            s.append(facevalue[j]);
            s.append(" of ");
            s.append(suit[i]);
            deck.push_back(s);
        }
    }
    printf("This is Alice the server.");

    sockfd=socket(AF_INET,SOCK_DGRAM,0);
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    servaddr.sin_port=htons(32002);
    bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
    len = sizeof(cliaddr);

    printf("\nInitializing cards...");
    for(i = 0 ; i < 52 ; i++)
    {
        cards[i] = i+47;
    }
    printf("[done]");

    while(money[0]>0)
    {
        printf("\nMoney in hand : %d\n",money[0]);
        bettingMoney[0] = 0;
        win[0] = 0;
        cheating[0] = 0;
        printf("\nGenerating Key...");
        recvfrom(sockfd,prime,sizeof(prime),0,(struct sockaddr *)&cliaddr,&len);
        key[0] = generateKey(prime[0]-1);
        printf("[done]");
        printf("\nShuffling Cards...");
        randomize(cards,52);
        printf("[done]");
        int temp[52];
        printf("\nEncrypting Cards...");
        for(int i = 0;i<52;i++)
        {
            temp[i] = encryption(cards[i],key[0],prime[0]);

        }
        printf("[done]");

        sendto(sockfd,temp,sizeof(temp),0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
        n = recvfrom(sockfd,mycards,sizeof(mycards),0,(struct sockaddr *)&cliaddr,&len);
        n = recvfrom(sockfd,opponentcards,sizeof(opponentcards),0,(struct sockaddr *)&cliaddr,&len);

        for(int i = 0 ; i < 5 ; i++ )
        {
            mycards[i] = decryption(mycards[i],modInverse(key[0],prime[0]-1),prime[0]);
            opponentcards[i] = decryption(opponentcards[i],modInverse(key[0],prime[0]-1),prime[0]);
        }

        sendto(sockfd,opponentcards,sizeof(opponentcards),0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));

        for(int i = 0; i < 5 ;i++)
        {
            cout << "\nCard[" << i+1 <<"] : " << deck.at(mycards[i]-47);
            printf("\nAmount to bet for this card : $");
            int bet = 10000;
            while(bet > (money[0] - bettingMoney[0]))
            {
                scanf("%d",&bet);
            }
            bettingMoney[0] += bet;
        }
        printf("Total betting amount : %d",bettingMoney[0]);
        inverseKey[0] = modInverse(key[0],prime[0]-1);

        sendto(sockfd,bettingMoney,sizeof(bettingMoney),0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
        sendto(sockfd,inverseKey,sizeof(inverseKey),0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
        sendto(sockfd,mycards,sizeof(mycards),0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
        n = recvfrom(sockfd,opponentMoney,sizeof(opponentMoney),0,(struct sockaddr *)&cliaddr,&len);
        n = recvfrom(sockfd,opponentDecrycards,sizeof(opponentDecrycards),0,(struct sockaddr *)&cliaddr,&len);
        n = recvfrom(sockfd,opponentKey,sizeof(opponentKey),0,(struct sockaddr *)&cliaddr,&len);

        for(int i = 0;i<5 ; i++)
        {
            if(decryption(opponentcards[i],opponentKey[0],prime[0]) != opponentDecrycards[i])
            {
                cheating[0] = 1;
            }

        }

        if(cheating[0] == 1 )
        {
            printf("Opponent did cheating while playing by changing cards");
        }
        else
        {
            for(int i = 0; i<5 ;i++)
            {
                if((mycards[i]-47)%13 > (opponentDecrycards[i]-47)%13)
                {
                    win[0]++;
                }
                else if((mycards[i]-47)%13 < (opponentDecrycards[i]-47)%13)
                {
                    win[0]--;
                }
                else
                {
                    if(mycards[i] > opponentDecrycards[i])
                    {
                        win[0]++;
                    }
                    else
                    {
                        win[0]--;
                    }
                }
            }
            printf("Win = %d",win[0]);
            if(win[0] > 0)
            {
                money[0] += opponentMoney[0];
            }
            else
            {
                money[0] -= bettingMoney[0];
            }
        }

    }
}

int power_mod(int number,int key, int n)
{
    int i,y = 1;
    for(i = 0; i< key ; i++)
    {
        if(y > n)
        {
            y = y%n;
        }
        y = y*number;

    }
    return y%n;
}

int encryption(int number,int key, int n)
{
    return power_mod(number,key,n);
}

int decryption(int number,int key, int n)
{
    return power_mod(number,key,n);
}

int modInverse(int a, int m) {
    a %= m;
    for(int x = 1; x < m; x++)
    {
        if((a*x) % m == 1)
            return x;
    }
}

void swap (int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

void randomize ( int arr[], int n )
{
    srand ( time(NULL) );
    for (int i = n-1; i > 0; i--)
    {
        int j = rand() % (i+1);
        swap(&arr[i], &arr[j]);
    }
}

int generateKey(int p)
{
    srand ( time(NULL) );
    int key;
    while(1)
    {
        key = rand()%p;
        if(key<p)
        {
            if(gcd(key,p)==1)
            {
                break;
            }
        }
    }
    return key;
}

int gcd(int x, int y)
{
    while(x!=y)
    {
        if(x>y)
            return gcd(x-y,y);
        else
            return gcd(x,y-x);
    }
    return x;
}
