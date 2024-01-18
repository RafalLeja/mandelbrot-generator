// Rafał Leja 2022

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define MIN(a,b) ((a) < (b) ? (a) : (b))

typedef enum difff{
    CHECK,
    ADDWHITE,
    ADDRED
} diff;
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

void printHelp();

void inputSequence(int argc, char const *argv[], Specs * param);

Pixel includedInSet(Point p, Point max, Point min, Specs param);

void calcScale(Point *bY, Point *bX, Specs * param);

long long int diffrence(diff arg, long long int * cntW, long long int * cntR);

Point imaginarySq(Point p);

int main(int argc, char const *argv[])
{
    Specs param = { -1, -1, "mandel-", 0.2, -1, {-0.743643887037158704752191506114774, -0.131825904205311970493132056385139} };
    inputSequence(argc, argv, &param);
    printf("w = %d, h = %d, pre = %s, zoom = %f, frames = %d, fx = %Lf, fy = %Lf\n", param.width, param.height, param.nameprefix, param.zoom, param.maxframes, param.focus.x, param.focus.y);
    Point max = {1.5, 1.25};
    Point min = {-2.5, -1.25};
    int activeFrame = 1;
    for (int frame = 0; frame < (param.maxframes > 0 ? param.maxframes : activeFrame); frame++){
        long long int cntr = 0;
        long long int cntw = 0;
        char * name = malloc((strlen(param.nameprefix)+5)*sizeof(char)); // TODO zmienić ze stałej
        sprintf(name, "%s%05d.ppm", param.nameprefix, frame); 
        FILE * plik = fopen(name, "w");
        if (!plik)
        {
            fprintf(stderr, "file %s could not be opened\n", name);
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
                diffrence((color.g > 0 ? ADDWHITE : ADDRED), &cntw, &cntr);
                //printf("%Lf %Lf\n", min.x, max.x);
                fprintf(plik, "%d %d %d ", color.r, color.g, color.b);
            }
            fprintf(plik, "\n");
        }
        fclose(plik);
        printf("frame %s done!\n", name);
        free(name);
        if (diffrence(CHECK, &cntw, &cntr) != 0)
        {
            activeFrame++;
        }
        calcScale(&max, &min, &param);
    }

    return 0;
}

void printHelp(){
    printf("Mandelbrot generator\n");
    printf("Usage: ./mandelbrot [options]\n");
    printf("Options:\n");
    printf("-h - prints this help\n");
    printf("--width <int> - width of the image\n");
    printf("--height <int> - height of the image\n");
    printf("--nameprefix <string> - prefix of the image name\n");
    printf("--zoom <float> - zoom of the image\n");
    printf("--maxframes <int> - max number of frames\n");
    printf("--focus <float>,<float> - focus of the image\n");
}

void inputSequence(int argc, char const *argv[], Specs * param){

    if (argc % 2 == 0)
    {
        if (argv[1][2] != 'h')
        {
            fprintf(stderr, "invalid number of arguments\n");
        }    
        printHelp();
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
            fprintf(stderr, "invalid argument %s", argv[i]);
            exit(1);
        }

        if (argv[i+1][0] == '-' && argv[i+1][1] == '-' && option != 'n')
        {
            fprintf(stderr, "argument without value %s\n", argv[i]);
            exit(1);
        }
        
        switch (option)
        {
        case 'w':
            i++;
            param->width = atoi(argv[i]);
            if (param->width == 0)
            {
                fprintf(stderr, "width must be greater than 0");
                exit(1);
            }
            break;
        
        case 'h':
            i++;
            param->height = atoi(argv[i]);
            if (param->height == 0)
            {
                fprintf(stderr, "height must be greater than 0");
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
                fprintf(stderr, "invalid \"focus\" ");
                printHelp();
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
    long double left = fabsl(min->x - param->focus.x);
    long double right = fabsl(param->focus.x - max->x);
    long double up = fabsl(param->focus.y - max->y);
    long double down = fabsl(min->y - param->focus.y);
    min->x += left*param->zoom;
    max->x -= right*param->zoom;
    min->y += down*param->zoom;
    max->y -= up*param->zoom;
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

long long int diffrence(diff arg, long long int * cntw, long long int * cntr){
    switch (arg)
    {
    case CHECK:
        long long out = MIN(*cntr, *cntw);
        cntw=0;
        cntr =0;
        return out;
        break;

    case ADDRED:
        *cntr += 1;
        break;
    
    case ADDWHITE:
        *cntw += 1;
        break;
    }
    return 0;
}


