#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef struct {
  long double x;
  long double y;
} Point;

typedef struct {
    int r;
    int g;
    int b;
} Pixel;

typedef struct {
    int width;
    int height;
    char * nameprefix; 
    float zoom;
    int maxframes;
    Point focus;
} Specs;

void inputSequence(int argc, char const *argv[], Specs * param);

Pixel includedInSet(Point p, int w, int h, Point bY, Point bX);

void calcScale(Point *bY, Point *bX, int frame, Specs * param);

Point imaginarySq(Point p);

int main(int argc, char const *argv[])
{
    Specs param = { -1, -1, "mandel-", 0.5, -1, {-35, 0} };
    inputSequence(argc, argv, &param);
    printf("w = %d, h = %d, pre = %s, zoom = %f, frames = %d, fx = %Lf, fy = %Lf\n", param.width, param.height, param.nameprefix, param.zoom, param.maxframes, param.focus.x, param.focus.y);
    Point max = {1.5, 1.25};
    Point min = {-2.5, -1.25};
    for (int frame = 0; frame < param.maxframes; frame++){
        calcScale(&max, &min, frame, &param);
    }

    return 0;
}

void inputSequence(int argc, char const *argv[], Specs * param){

    if (argc % 2 == 0)
    {
        fprintf(stderr, "bledna ilosc argumentow");
        exit(1);
    }
    char options[6][15] = {"--width", "--height", "--nameprefix", "--zoom", "--maxframes", "--focus"};


    for (int i = 1; i < argc; i++)
    {
        char option = '\0';
        for (int j = 0; j < 6; j++)
        {
            if (strcmp(argv[i], options[j]) == 0)
            {
                option = options[j][2];
                break;
            }
        }

        if (option == '\0'){
            fprintf(stderr, "bledny argument %s", argv[i]);
            exit(1);
        }

        if (argv[i+1][0] == '-' && argv[i+1][1] == '-' && option != 'n')
        {
            fprintf(stderr, "brak podania wartosci dla argumentu %s\n", argv[i]);
            exit(1);
        }
        
        switch (option)
        {
        case 'w':
            i++;
            param->width = atoi(argv[i]);
            if (param->width == 0)
            {
                fprintf(stderr, "szerokosc musi byc wieksza od 0");
                exit(1);
            }
            break;
        
        case 'h':
            i++;
            param->height = atoi(argv[i]);
            if (param->height == 0)
            {
                fprintf(stderr, "wysokosc musi byc wieksza od 0");
                exit(1);
            }
            break;

        case 'n':
            i++;
            param->nameprefix = strdup(argv[i]);
            break;

        case 'z':
            i++;
            param->zoom = atof(argv[i]);
            break;

        case 'm':
            i++;
            param->maxframes = atoi(argv[i]);
            break;

        case 'f':
            i++;
            char *splitIdx = strchr(argv[i], ',');
            if (splitIdx == NULL)
            {
                fprintf(stderr, "blad parametru --focus X,Y");
                exit(1);
            }
            char * buff = malloc((splitIdx - argv[i])*sizeof(char));
            strncpy(buff, argv[i], (splitIdx - argv[i]));
            param->focus.x = atoi(buff);
            param->focus.y = atoi(argv[i]+(splitIdx - argv[i])+1);
            free(buff);
            break;

        default:
            break;
        }
    }
    
    if (param->height < 0 && param->width > 0)
    {
        param->height = param->width * 5/8;
    }else if (param->width < 0 && param->height > 0)
    {
        param->width = param->height * 8/5;
    }else if (param->width < 0 && param->height < 0)
    {
        param->width = 1920;//mniej
        param->height = 1200;
    }
}

void calcScale(Point *max, Point *min, int frame, Specs * param){
    long double wide = lfabs(max->x - min->x); // calosc - kawalek od punktu
    long double high = lfabs(max->y - min->y);
    
}
