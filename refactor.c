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

Pixel includedInSet(Point p, Point max, Point min, Specs param);

void calcScale(Point *bY, Point *bX, Specs * param);

long double diffrence(int r);

Point imaginarySq(Point p);

int main(int argc, char const *argv[])
{
    Specs param = { -1, -1, "mandel-", 0.75, -1, {0, 0} };
    inputSequence(argc, argv, &param);
    printf("w = %d, h = %d, pre = %s, zoom = %f, frames = %d, fx = %Lf, fy = %Lf\n", param.width, param.height, param.nameprefix, param.zoom, param.maxframes, param.focus.x, param.focus.y);
    Point max = {1.5, 1.25};
    Point min = {-2.5, -1.25};
    int activeFrame = 1;
    for (int frame = 0; frame < (param.maxframes > 0 ? param.maxframes : activeFrame); frame++){
        diffrence(-2);
        char * name = malloc((strlen(param.nameprefix)+5)*sizeof(char)); // TODO zmienić ze stałej
        sprintf(name, "%s%05d.ppm", param.nameprefix, frame); 
        FILE * plik = fopen(name, "w");
        if (!plik)
        {
            fprintf(stderr, "błąd otwierania pliku");
            return 1;
        }
        fprintf(plik, "P3\n");
        fprintf(plik, "%d %d\n", param.width, param.height);
        fprintf(plik, "255\n");
        for (int y = 0; y < param.height; y++)
        {
            for (int x = 0; x < param.width; x++)
            {
                Point point = {x, y};
                Pixel color = includedInSet(point, max, min, param);
                diffrence(color.r);
                //printf("%Lf %Lf\n", min.x, max.x);
                fprintf(plik, "%d %d %d ", color.r, color.g, color.b);
            }
            fprintf(plik, "\n");
        }
        fclose(plik);
        free(name);
        if (diffrence(-1) != 0)
        {
            activeFrame++;
        }
        //printf("%Lf\n", diffrence(-1));
        calcScale(&max, &min, &param);
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
            param->nameprefix = strcpy(param->nameprefix, argv[i]);
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
            param->focus.x = atof(buff);
            param->focus.y = atof(argv[i]+(splitIdx - argv[i])+1);
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
    if (param->focus.x == 0 && param->focus.y == 0)
    {
        param->focus.x = -1.400005;
    }
    
}

void calcScale(Point *max, Point *min, Specs * param){
    // if o krawędziach
    long double left = fabsl(min->x - param->focus.x);
    long double right = fabsl(param->focus.x - max->x);
    long double up = fabsl(param->focus.y - max->y);
    long double down = fabsl(min->y - param->focus.y);
    //printf("%Lf %Lf %Lf %Lf\n", left, right, up, down);
    min->x += left/param->zoom;
    max->x -= right/param->zoom;
    min->y += down/param->zoom;
    max->y -= up/param->zoom;
}

Pixel includedInSet(Point p, Point max, Point min, Specs param){
    Pixel color = {0, 0, 0};
    Point imaginaryPoint;
    imaginaryPoint.x = (p.x * (fabsl(min.x - max.x)/param.width) + min.x);
    imaginaryPoint.y = (p.y * (fabsl(min.y - max.y)/param.height) + min.y);
    
    Point z = {0, 0};
    int i; 
    for (i = 0; i<2000 && sqrt(pow(z.x,2)+ pow(z.y,2)) < 2; i++)
    {
        Point sqr = imaginarySq(z);
        z.x = sqr.x + imaginaryPoint.x; 
        z.y = sqr.y + imaginaryPoint.y; 
    }
    if (sqrt(pow(z.x,2)+ pow(z.y,2)) >= 2)
    {
        color.r = abs((255/i) - 255);
        return color;
    }
    color.r = 255;
    color.g = 255;
    color.b = 255;
    return color;
}

Point imaginarySq(Point p){
    Point o = { pow(p.x, 2) - pow(p.y, 2), 2*p.x*p.y};
    return o; 
}

long double diffrence(int r){
    static long double min;
    static long double max;
    if (r == -1)
    {
        return (max - min);
    }else if (r == -2)
    {
        min = 256;
        max = -1;
        return 0;
    }
    if(r < min){
        min = r;
    }
    if (r > max)
    {
        max = r;
    }
    return 0;
}


