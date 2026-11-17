/*
 * CHIP-8 에뮬레이터 — Step 4: 첫 명령어 6개로 IBM로고 그리기
 *
 */
#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>

#include "chip8.h"


#define SCALE         10   /* 화면 확대 배율: 64x32 -> 640x320 */
const uint32_t PixelOn = 0xffffffff;
const uint32_t PixelOff = 0xff000000;

static void DisplayTest(chip8* machine);
static void UpdatePixel(const chip8* machine, uint32_t* pixel);

int main(int argc, char *argv[])
{

    chip8 machine;
    uint32_t pixels[SCREENWIDTH * SCREENHEIGHT];
    int result = 0;
   
    if (argc < 2)
    {
        fprintf(stderr, "Usage : chip8.exe romfilepath\n");
        return 1;
    }

    /* 1. SDL 초기화 (영상) */
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init 실패: %s\n", SDL_GetError());
        return 1;
    }

    /* 2. 창 만들기 */
    SDL_Window *window = SDL_CreateWindow(
        "CHIP-8",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREENWIDTH * SCALE, SCREENHEIGHT * SCALE,
        0);
    if (!window) {
        fprintf(stderr, "SDL_CreateWindow 실패: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    /* 3. 렌더러 만들기 (화면에 그리는 도구). PRESENTVSYNC = 모니터 주사율에 맞춰 대기 */
    SDL_Renderer *renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        fprintf(stderr, "SDL_CreateRenderer 실패: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Texture* screenTexture = SDL_CreateTexture(renderer, 
        SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, 
        SCREENWIDTH, SCREENHEIGHT);
    if (!screenTexture)
    {
        fprintf(stderr, "SDL_CreateTexture 실패: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return 1;
    }

    chip8_init(&machine);

    if (!chip8_loadRom(&machine, argv[1]))
    {
        fprintf(stderr,"Rom Load Failed!!\n");
        result = 1;
        goto release;
    }
    OPINDEX opIndex = UNKNOWN;
    chip8_Testdecode(&machine);
    //DisplayTest(&machine);

    /* 4. 메인 루프 */
    bool running = true;
    while (running) {
        /* 4-1. 이벤트 처리: 창 닫기, 키 입력 */
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN &&
                       event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }
        }

    
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        /*
        * 4-2  명령어 실행
        */
        for (int i = 0; i < CPUCYCLECOUNT; ++i)
        {
           opIndex = chip8_CPUCycle(&machine);
           
           if (opIndex == UNKNOWN)
           {
               result = 1;
               goto release;
           }
          
        }

       

        /*
        * 4-3 화면그리기
        */
        UpdatePixel(&machine, pixels);
        SDL_UpdateTexture(screenTexture, NULL, pixels, sizeof(pixels[0]) * SCREENWIDTH);
        SDL_RenderCopy(renderer, screenTexture, NULL, NULL);

        SDL_RenderPresent(renderer);

        /* 4-4. 약 60fps 유지 (VSync가 꺼진 환경 대비) */
        SDL_Delay(16);
    }

release:
    /* 5. 정리 */
    SDL_DestroyTexture(screenTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return result;
}


void DisplayTest(chip8* machine)
{
    
    int i = 0;
    int j = 0;
 

    for (i = 0; i < SCREENHEIGHT; ++i)
    {
        for (j = 0; j < SCREENWIDTH; ++j)
        {
            int index = i * SCREENWIDTH + j;
       
            machine->display[index] = (index + i) %2;
        }

    }
}

void UpdatePixel(const chip8* machine, uint32_t* pixel)
{

    for (int i = 0; i < SCREENHEIGHT * SCREENWIDTH; ++i)
    {
        pixel[i] = machine->display[i] ? PixelOn : PixelOff;
    }
}
