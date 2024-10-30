/*
    MADE ON JANUARY 2020
*/

#include <windows.h>
#include <assert.h>
#include "include/obfuscate.h"

static void AccurateSleep(float ms, bool revert_changes = false)
{
    constexpr ULONG min_resolution = static_cast<ULONG>(0.5f * 10000);
    if (ms < 0.5f) ms = 0.5f;

    const char* ntdll = AY_OBFUSCATE("ntdll.dll");
    const char* ntdelay = AY_OBFUSCATE("NtDelayExecution");
    const char* zwtimer = AY_OBFUSCATE("ZwSetTimerResolution");
    static NTSTATUS(__stdcall * NtDelayExecution)(BOOL Alertable, PLARGE_INTEGER DelayInterval) = (NTSTATUS(__stdcall*)(BOOL, PLARGE_INTEGER)) GetProcAddress(GetModuleHandle((ntdll)), (ntdelay));
    static NTSTATUS(__stdcall * ZwSetTimerResolution)(IN ULONG RequestedResolution, IN BOOLEAN Set, OUT PULONG ActualResolution) = (NTSTATUS(__stdcall*)(ULONG, BOOLEAN, PULONG)) GetProcAddress(GetModuleHandle((ntdll)), (zwtimer));

    static bool once = false;
    if (!once) {
        ULONG actual_resolution;
        once = ZwSetTimerResolution(min_resolution, true, &actual_resolution) >= 0;
        assert(once);
    }

    LARGE_INTEGER time{ 0 };
    time.QuadPart = -1 * static_cast<LONGLONG>(ms * 10000.0f);
    NtDelayExecution(false, &time);

    if (once && revert_changes) {
        ULONG actual_resolution;
        once = ZwSetTimerResolution(min_resolution, false, &actual_resolution) >= 0;
        assert(once);
    }
}

#include "../../imgui/imgui.h"
#include "../../imgui/imgui_impl_glfw.h"
#include "../../imgui/imgui_impl_opengl3.h"
#include <stdio.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <functional>
#include <sstream>
#include <windows.h>
#include <Lmcons.h>
#include <time.h>
#include <intrin.h>
#include <winsock.h>
#include <shellapi.h>
#include <wdbgexts.h>
#include <Dbgeng.h>
#include <Mmsystem.h>
#include <chrono>
#include <algorithm>
#include <curl/curl.h>
//#include <SOIL/SOIL.h> //for image dumping
#include <fstream> //for sound/image dumping
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "include/stb_image.h"
#include "include/stb_image_resize.h"
#include "include/random_compile.h"
#include "include/md5.h"
#include "include/icon_image.h"
#include "include/click_sounds.h"
#include "include/humanized_clicks_delays.h"
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "legacy_stdio_definitions")
#define WIN32_LEAN_AND_MEAN
#include <iostream>  
#include <string>  
#include <TlHelp32.h>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/prettywriter.h"

//#define DEBUG

#define WEB_ROOT "http://peces.educationhost.cloud" //try to make it https / proxy

using namespace std; //lol

char* VERSION = AY_OBFUSCATE("1.12", (char)33 + abs(RAND) % (125-32));
char license_reciever[50];
char key[50];
char cmdname[50];
char settingsname[50];

// customize (www.utf8-chartable.de)
// KILLSWITCH IS: "BEBE" and it will work, anything else and it will die
char* kswitch = AY_OBFUSCATE("abcd", (char)33 + abs(RAND) % (125-32));


float min_CPS = 12.5;
float max_CPS = 15;
float min_BCPS = 4.5;
float max_BCPS = 7.5;
int jitter_strength = 3;

char* click_types[] = {AY_OBFUSCATE("Jitterclick", (char)33 + abs(RAND) % (125-32)), AY_OBFUSCATE("Butterfly", (char)33 + abs(RAND) % (125-32)), AY_OBFUSCATE("Low CPS", (char)33 + abs(RAND) % (125-32)), AY_OBFUSCATE("g303", (char)33 + abs(RAND) % (125 - 32)), AY_OBFUSCATE("g502", (char)33 + abs(RAND) % (125 - 32)), AY_OBFUSCATE("GPro", (char)33 + abs(RAND) % (125 - 32)), AY_OBFUSCATE("Custom", (char)33 + abs(RAND) % (125-32)) };

int toggle_key = VK_F4; //f4
int blockhit_key = VK_F12; //f12
int jittr_key = VK_F10; //f10
int streamer_key = VK_F6; //f6
int inv_key = 0x45; //E
int lagswitch_key = VK_F7; //F7
int destr_key = VK_F8; //f8

HHOOK mouseHook;
HHOOK keyboardHook;
HANDLE hConsole;
POINT mousepos;

BOOLEAN havingFun;
BOOLEAN mouseDown;
BOOLEAN RmouseDown;
BOOLEAN interrupt;
BOOLEAN destruct;
HWND active_window;
bool silent_load;
bool running_as_admin;
bool humanized_clicks;
bool streamermode;
bool jitter;
bool blockhit;
bool no_left_click_blockhit;
bool multipleclick;
bool canmulticlick;
bool no_clickdown;
bool show_hotkeys_gui;
bool show_presets_gui;
bool show_color_config;
bool hotkeys1, hotkeys2, hotkeys3, hotkeys4, hotkeys5, hotkeys6, hotkeys7;
bool chroma;
bool clicksound;
bool inc_increase;
bool exhaust;
bool first_time_inc;
bool first_time_exhaust;
bool done_inc;
bool currently_exhausting;
bool cps_drops;
bool currently_incpsdrop;
bool cps_spikes;
bool currently_incpsspike;
bool lagSwitch;
bool prev_lagswitch;
bool onlyinmc;
bool isonmc;
bool sound_on_activate;
bool invert_on_toggle;
int lagSwitch_val;
string lag_rule_name;
float inc_increase_val;
float exhaust_timer;
float exhaust_val;
int click_type_selected;
char customclick[70];
char custombackgroundbuf[70];
char prev_bg[70];
bool custombackground;
bool up_x, up_y, up_z;
bool disable_while_inv;
bool hide;
bool in_inv;
bool sec_test = false;
float prev_maxCPS, prev_maxBCPS;
float prev_minCPS, prev_minBCPS;
float multipleclick_count;
int cps_drops_percentage;
int cps_drops_val;
int cps_spikes_percentage;
int cps_spikes_val;
int bg_image_width = 0;
int bg_image_height = 0;
GLuint bg_image_texture = 0;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

struct timeval times;
long long ms1, ms2;

char toggle_btn_value[12];
char blockhit_btn_value[12];
char jittr_btn_value[12];
char streamer_btn_value[12];
char inv_btn_value[12];
char lagswitch_btn_value[12];
char destr_btn_value[12];

double realcps;
double prev_realcps = -1;
double realBcps;
double inc_increase_CPS = 0;
double exhaust_CPS = 0;
double cps_drop_CPS = 0;
double cps_spike_CPS = 0;
double CPS = (max_CPS + min_CPS) / 2;
double BCPS = (max_BCPS + min_BCPS) / 2;
long long totalCPS;
clock_t first_time_cps;
float cpsvalues[20] = { 13, 15, 16, 15, 12, 13, 15, 16,12, 9, 10, 15, 17, 12, 12, 15, 11, 10, 13, 13 };

char* b64_c = AY_OBFUSCATE("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/", (char)33 + abs(RAND) % (125-32));

#define VOLUME_UP 10
#define VOLUME_DOWN 20

int main(int argc, char** argv);
int startFun();
LRESULT CALLBACK MouseCallBack(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK KeyboardCallBack(int nCode, WPARAM wParam, LPARAM lParam);
DWORD WINAPI randomSeedThread(LPVOID lParam);
DWORD WINAPI HookThread(LPVOID lParam);
DWORD WINAPI ClickThread(LPVOID lParam);
DWORD WINAPI BlockHitThread(LPVOID lParam);
DWORD WINAPI GradualClickThread(LPVOID lParam);
DWORD WINAPI MultipleClickThread(LPVOID lParam);
DWORD WINAPI JitterThread(LPVOID lParam);
DWORD WINAPI CPSDropSpikeThread(LPVOID lParam);
DWORD WINAPI lagSwitchThread(LPVOID lParam);
DWORD WINAPI manageHotkeysThread(LPVOID lParam);
DWORD WINAPI authThread(LPVOID lParam);
int stopFun();
int drawGui();
int saveSettings();
int loadSettings();
ImVec4 handleChroma(ImVec4 color);
int getPassword();
int getLicenseReciever();
int getCmdName();
int getSettingsFile();
int canHaveFun();
int checkFunEnabled();
int checkUpdates();
int changeVolume(int chosen_sound, int option, int cycles);
int invertColor();
bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);
char* getKeyName(int key);
bool IsProcessRunning(const char* processName);
bool SymlinkExists(const char* symlink);
string getHwId();
string encode(unsigned char const* bytes_to_encode, unsigned int in_len);
string decode(std::string const& encoded_string);
int gettimeofday(struct timeval* tp, struct timezone* tzp);
int OnlyNumbersTextFilter(ImGuiTextEditCallbackData* data);
string random_string(size_t length);
bool IsElevated();
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
PVOID GetPEB();
int CheckNtGlobalFlag();
int freeStuff();
int restartProcesses();
int system_no_output(const char* cmd);

int setDefault();
int setLegit();
int setTryhard();


int main(int argc, char** argv)
{

    // set to black on white
    // colors are 0=black 1=blue 2=green and so on to 15=white  
    // colorattribute = foreground + background * 16
    SetConsoleTextAttribute(hConsole, 240);
    //auth
    //ImGui::ProgressBar() for login

    running_as_admin = IsElevated();

    if (!running_as_admin)
    {
        printf(AY_OBFUSCATE("\nPlease run as admin.\n", (char)33 + abs(RAND) % (125 - 32)));
        exit(0);
    }

    startFun();

    return 0;
}

int startFun()
{
    #if !defined(DEBUG)
    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
    #endif
    totalCPS = 0;
    mouseDown = false;
    havingFun = false;
    humanized_clicks = false;
    no_left_click_blockhit = false;
    blockhit = false;
    multipleclick = false;
    multipleclick_count = 0.3f;
    canmulticlick = true;
    streamermode = false;
    hide = false;
    custombackgroundbuf[0] = 0;
    custombackground = false;
    up_x = true;
    up_y = false;
    up_z = true;
    silent_load = true; //people cant open?
    disable_while_inv = false;
    in_inv = false;
    chroma = false;
    inc_increase = false;
    exhaust = false;
    cps_drops = false;
    cps_spikes = false;
    first_time_inc = true;
    first_time_exhaust = true;
    done_inc = false;
    currently_exhausting = false;
    currently_incpsdrop = false;
    currently_incpsspike = false;
    lagSwitch = false;
    prev_lagswitch = false;
    lagSwitch_val = 30;
    inc_increase_val = 1.0f;
    exhaust_val = 6.0f;
    exhaust_timer = 13.0f;
    cps_drops_percentage = 15;
    cps_spikes_percentage = 15;
    cps_drops_val = 20;
    cps_spikes_val = 20;
    clicksound = false;
    click_type_selected = 0;
    hotkeys1 = false;
    hotkeys2 = false;
    hotkeys3 = false;
    hotkeys4 = false;
    hotkeys5 = false;
    hotkeys6 = false;
    hotkeys7 = false;
    show_color_config = false;
    show_hotkeys_gui = false;
    show_presets_gui = false;
    jitter = false;
    no_clickdown = false;
    onlyinmc = false;
    isonmc = false;
    sound_on_activate = false;
    invert_on_toggle = false;
    destruct = false;
    interrupt = false;
    srand(time(NULL));
    gettimeofday(&times, NULL);
    ms1 = 0;

    loadSettings();

    strncpy(toggle_btn_value, getKeyName(toggle_key), sizeof(toggle_btn_value));
    strncpy(blockhit_btn_value, getKeyName(blockhit_key), sizeof(blockhit_btn_value));
    strncpy(jittr_btn_value, getKeyName(jittr_key), sizeof(jittr_btn_value));
    strncpy(streamer_btn_value, getKeyName(streamer_key), sizeof(streamer_btn_value));
    strncpy(inv_btn_value, getKeyName(inv_key), sizeof(inv_btn_value));
    strncpy(lagswitch_btn_value, getKeyName(lagswitch_key), sizeof(lagswitch_btn_value));
    strncpy(destr_btn_value, getKeyName(destr_key), sizeof(destr_btn_value));

    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&randomSeedThread, NULL, 0, 0);
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&HookThread, NULL, 0, 0);
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&ClickThread, NULL, 0, 0);
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&BlockHitThread, NULL, 0, 0);
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&GradualClickThread, NULL, 0, 0);
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&MultipleClickThread, NULL, 0, 0);
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&JitterThread, NULL, 0, 0);
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&CPSDropSpikeThread, NULL, 0, 0);
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&lagSwitchThread, NULL, 0, 0);
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&manageHotkeysThread, NULL, 0, 0);
    
    drawGui();

    stopFun();
    return 0;
}

LRESULT CALLBACK MouseCallBack(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (license_reciever == "test")
        stopFun();

    PMSLLHOOKSTRUCT pMouse = (PMSLLHOOKSTRUCT)lParam;
    if (pMouse)
    {
        if (WM_MOUSEMOVE != wParam)
        {
            //anticheat bypass
            //if ((pMouse->flags & LLMHF_INJECTED))
            //{
            //    pMouse->flags &= ~LLMHF_INJECTED;
            //}
            // if this returns 0, we know it's a real click. see: https://msdn.microsoft.com/en-us/library/windows/desktop/ms644970(v=vs.85).aspx
            if (!pMouse->flags)
            {
                switch (wParam)
                {
                case WM_LBUTTONDOWN:
                    gettimeofday(&times, NULL);
                    if (!mouseDown)
                    {
                        ms2 = ms1;
                        first_time_cps = clock();
                    }
                    else
                        ms2 = times.tv_sec * 1000 + times.tv_usec / 1000;
                    ms1 = times.tv_sec * 1000 + times.tv_usec / 1000;
                    mouseDown = true;
                    break;
                case WM_LBUTTONUP:
                    canmulticlick = true;
                    // set currentlyexhausting to false;
                    mouseDown = false;
                    break;
                case WM_RBUTTONDOWN:
                    RmouseDown = true;
                    break;
                case WM_RBUTTONUP:
                    RmouseDown = false;
                    break;
                }
            }
        }
    }
    return CallNextHookEx(mouseHook, nCode, wParam, (LPARAM)pMouse);
}

LRESULT CALLBACK KeyboardCallBack(int nCode, WPARAM wParam, LPARAM lParam)
{
    PKBDLLHOOKSTRUCT keyStruct = (PKBDLLHOOKSTRUCT)lParam;
    if (keyStruct
        && WM_KEYUP == wParam
        && toggle_key == keyStruct->vkCode)
    {

        havingFun = !havingFun;
        if (sound_on_activate)
            PlaySound(toggle_sound, NULL, SND_MEMORY | SND_ASYNC);
        if (invert_on_toggle)
            invertColor();
    }

    else if (keyStruct
        && WM_KEYUP == wParam
        && destr_key == keyStruct->vkCode)
    {
        destruct = true;
        stopFun();
    }

    else if (keyStruct
        && WM_KEYUP == wParam
        && streamer_key == keyStruct->vkCode)
    {

        streamermode = !streamermode;
    }

    else if (keyStruct
        && WM_KEYUP == wParam
        && inv_key == keyStruct->vkCode)
    {

        in_inv = !in_inv;
    }

    else if (keyStruct
        && WM_KEYUP == wParam
        && blockhit_key == keyStruct->vkCode)
    {

        blockhit = !blockhit;
    }

    else if (keyStruct
        && WM_KEYUP == wParam
        && jittr_key == keyStruct->vkCode)
    {

        jitter = !jitter;
    }

    else if (running_as_admin 
         && keyStruct
         && WM_KEYUP == wParam
         && lagswitch_key == keyStruct->vkCode)
    {
        lagSwitch = !lagSwitch;
    }

    else if (keyStruct
        && WM_KEYUP == wParam
        && (VK_OEM_PLUS == keyStruct->vkCode || 107 == keyStruct->vkCode)
        && GetAsyncKeyState(VK_SHIFT))
    {
        min_CPS += 0.5;
        max_CPS += 0.5;
    }

    else if (keyStruct
        && WM_KEYUP == wParam
        && (VK_OEM_MINUS == keyStruct->vkCode || 109 == keyStruct->vkCode)
        && GetAsyncKeyState(VK_SHIFT))
    {

        min_CPS -= 0.5;
        max_CPS -= 0.5;
    }

    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

DWORD WINAPI randomSeedThread(LPVOID lParam)
{
    while (1)
    {
        int randomi = rand() % ((1 - 0) + 1) + 0; //1 or 0
        if (randomi == 0)
            srand(time(0));
        else
            srand(GetTickCount64());

        AccurateSleep(rand() % (15000 - 1000 + 1) + 1000); //wait enough time to get next seed (between 1s and 15s)
    }
}

DWORD WINAPI HookThread(LPVOID lParam)
{
    mouseHook = SetWindowsHookEx(WH_MOUSE_LL, &MouseCallBack, NULL, NULL);
    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, &KeyboardCallBack, NULL, NULL);

    // Infinite loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(mouseHook);
    UnhookWindowsHookEx(keyboardHook);
    return 0;
}

DWORD WINAPI ClickThread(LPVOID lParam)
{
    double random;
    int index = 1;
    int delay;
    while (1)
    {
        if (havingFun && (mouseDown || no_clickdown) && isonmc && (!disable_while_inv || (disable_while_inv && !in_inv)))
        {
            if (humanized_clicks)
            {
                mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, NULL, NULL);
                delay = humanized_clicks_delays[index++];
                if (rand() % 5 == 0)
                    delay *= 1.05;
                AccurateSleep(delay);
                if (index >= IM_ARRAYSIZE(humanized_clicks_delays))
                    index = 0;

                mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, NULL, NULL);
                delay = humanized_clicks_delays[index++];
                if (rand() % 5 == 0)
                    delay *= 0.95;
                AccurateSleep(delay);
                if (index >= IM_ARRAYSIZE(humanized_clicks_delays))
                    index = 1;
            }
            else
            { 
                mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, NULL, NULL);
                random = ((double)rand() / RAND_MAX) * ((max_CPS - CPS) - (min_CPS - CPS)) + (min_CPS - CPS);
                realcps = (CPS + random + inc_increase_CPS + exhaust_CPS + cps_drop_CPS + cps_spike_CPS) * 1.05;
                realcps = realcps < 0 ? 0 : realcps;
                AccurateSleep(1000 / ((realcps) * 2));

                random = ((double)rand() / RAND_MAX) * ((max_CPS - CPS) - (min_CPS - CPS)) + (min_CPS - CPS);
                realcps = (CPS + random + inc_increase_CPS + exhaust_CPS + cps_drop_CPS + cps_spike_CPS) * 0.95;
                mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, NULL, NULL);
                realcps = realcps < 0 ? 0 : realcps;
                AccurateSleep(1000 / ((realcps) * 2));
            }
            if (clicksound)
            {
                switch (click_type_selected)
                {
                case 0:
                    PlaySound(jitterclick_click_sound, NULL, SND_MEMORY | SND_ASYNC);
                    break;
                case 1:
                    PlaySound(butterfly_click_sound, NULL, SND_MEMORY | SND_ASYNC);
                    break;
                case 2:
                    PlaySound(low_cps_click_sound, NULL, SND_MEMORY | SND_ASYNC);
                    break;
                case 3:
                    PlaySound(g303_click_sound, NULL, SND_MEMORY | SND_ASYNC);
                    break;
                case 4:
                    PlaySound(g502_click_sound, NULL, SND_MEMORY | SND_ASYNC);
                    break;
                case 5:
                    PlaySound(gpro_click_sound, NULL, SND_MEMORY | SND_ASYNC);
                    break;
                case IM_ARRAYSIZE(click_types) - 1:
                    PlaySound(customclick, NULL, SND_ASYNC);
                    break;
                default:
                    break;
                }
            }

            totalCPS++;
        }
        else
            realcps = 0;

        AccurateSleep(1); //dont kill CPU
    }
}

DWORD WINAPI BlockHitThread(LPVOID lParam)
{
    double random;
    while (1)
    {
        if (blockhit && havingFun && RmouseDown && isonmc && (no_left_click_blockhit || mouseDown) && (!disable_while_inv || (disable_while_inv && !in_inv)))
        {
            random = ((double)rand() / RAND_MAX) * ((max_BCPS - BCPS) - (min_BCPS - BCPS)) + (min_BCPS - BCPS);
            realBcps = BCPS + random;
            mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, NULL, NULL);
            realBcps = realBcps < 0 ? 0 : realBcps;
            AccurateSleep(1000 / ((realBcps) * 2));

            random = ((double)rand() / RAND_MAX) * ((max_BCPS - BCPS) - (min_BCPS - BCPS)) + (min_BCPS - BCPS);
            realBcps = BCPS + random;
            mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, NULL, NULL);
            realBcps = realBcps < 0 ? 0 : realBcps;
            AccurateSleep(1000 / ((realBcps) * 2));

            if (clicksound)
            {
                switch (click_type_selected)
                {
                case 0:
                    PlaySound(jitterclick_click_sound, NULL, SND_MEMORY | SND_ASYNC);
                    break;
                case 1:
                    PlaySound(butterfly_click_sound, NULL, SND_MEMORY | SND_ASYNC);
                    break;
                case 2:
                    PlaySound(low_cps_click_sound, NULL, SND_MEMORY | SND_ASYNC);
                    break;
                case 3:
                    PlaySound(g303_click_sound, NULL, SND_MEMORY | SND_ASYNC);
                    break;
                case 4:
                    PlaySound(g502_click_sound, NULL, SND_MEMORY | SND_ASYNC);
                    break;
                case 5:
                    PlaySound(gpro_click_sound, NULL, SND_MEMORY | SND_ASYNC);
                    break;
                case 6:
                    PlaySound(customclick, NULL, SND_ASYNC);
                    break;
                default:
                    break;
                }
            }
        }
        else
            realBcps = 0;

        AccurateSleep(1); //dont kill CPU
    }
}

DWORD WINAPI GradualClickThread(LPVOID lParam)
{
    double temp_exhaust_CPS, temp_inc_increase_CPS;
    clock_t ms_start, started_exhausting;
    float adj_clock;
    while (1)
    {
        if (havingFun && (mouseDown || no_clickdown) && isonmc && (!disable_while_inv || (disable_while_inv && !in_inv)))
        {
            /* Gradual Increase BEGIN */
            if (done_inc && !inc_increase)
                done_inc = false;

            if (!done_inc && inc_increase)
            {
                //done coz threads
                temp_inc_increase_CPS = inc_increase_CPS;
                if (first_time_inc)
                {
                    ms_start = clock();
                    first_time_inc = false;
                }

                adj_clock = ms_start + inc_increase_val * 1000;
                temp_inc_increase_CPS = (((clock() - adj_clock) * (min_CPS * 1.2)) / (inc_increase_val * 1000)); //magic

                inc_increase_CPS += ((double)rand() / RAND_MAX) * ((max_CPS - CPS) - (min_CPS - CPS)) + (min_CPS - CPS);

                if (temp_inc_increase_CPS < -(CPS / 2))
                    temp_inc_increase_CPS = -(CPS / 2) + ((double)rand() / RAND_MAX) * ((max_CPS - CPS) - (min_CPS - CPS)) + (min_CPS - CPS);

                if (temp_inc_increase_CPS >= 0.0)
                    done_inc = true;

                inc_increase_CPS = temp_inc_increase_CPS;
            }
            /* Gradual Increase END */

            /* exhaust BEGIN */
            if (exhaust)
                currently_exhausting = (clock() - ((first_time_cps + (exhaust_timer * 1000))) > 0) ? true : false;

            if (currently_exhausting)
            {
                //done coz threads
                temp_exhaust_CPS = exhaust_CPS;

                if (first_time_exhaust)
                {
                    started_exhausting = clock();
                    first_time_exhaust = false;
                }

                temp_exhaust_CPS = -(((clock() - started_exhausting) * (min_CPS * 1.2)) / (exhaust_val * 1000));

                temp_exhaust_CPS += ((double)rand() / RAND_MAX) * ((max_CPS - CPS) - (min_CPS - CPS)) + (min_CPS - CPS);

                if (temp_exhaust_CPS < -(CPS / 2))
                {
                    temp_exhaust_CPS = -(CPS / 2) + ((double)rand() / RAND_MAX) * ((max_CPS - CPS) - (min_CPS - CPS)) + (min_CPS - CPS);
                }

                exhaust_CPS = temp_exhaust_CPS;
            }
            /* exhaust END */
        }
        else
        {
            first_time_inc = true;
            inc_increase_CPS = 0.0; //reset
            done_inc = false;
            currently_exhausting = false;
            first_time_exhaust = true;
            exhaust_CPS = 0.0;
        }

        AccurateSleep(1);
    }
}

DWORD WINAPI MultipleClickThread(LPVOID lParam)
{
    float random1;
    double random;
    int random2;
    long long sleep;
    long long sleepprc;
    while (1)
    {
        if (multipleclick && canmulticlick && mouseDown && isonmc)
        {
            random1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            
            if (random1 <= multipleclick_count) 
            {
                sleep = (ms1 - ms2) / 2;

                if (sleep == 0)
                {
                    random2 = (rand() % 40) + 1;
                    sleep = random2;
                }
                else if (sleep > 350)
                {
                    random2 = (rand() % 350) + 260;
                    sleep = random2;
                }

                sleepprc = (sleep * 20) / 100;

                // between 20% and 5%
                random = (rand() % (int)((sleep * 20) / 100) + ((sleep * 5) / 100));
                random2 = (rand() % 1) + 0;

                if (random2 == 1)
                    random *= -1;

                mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, NULL, NULL);
                AccurateSleep(sleep + random);

                random = (rand() % (int)(sleep * 1.2)) + (sleep * 0.8);
                random2 = (rand() % 1) + 0;

                if (random2 == 1)
                    random *= -1;

                mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, NULL, NULL);
                AccurateSleep(sleep + random);

                canmulticlick = false; //stop it from behaving like autoclicker
            }
        }
        
        AccurateSleep(1); //dont kill CPU
    }
}

DWORD WINAPI JitterThread(LPVOID lParam)
{
    POINT p;
    double random;
    int offsetx, offsety;

    while (1)
    {
        if (!jitter)
        {
            AccurateSleep(30);
            continue;
        }
        
        AccurateSleep(10);
        if (havingFun && (mouseDown || no_clickdown) && isonmc && (!disable_while_inv || (disable_while_inv && !in_inv)))
        {
            GetCursorPos(&p);
            random = ((double)rand() / RAND_MAX) * ((max_CPS - CPS) - (min_CPS - CPS)) + (min_CPS - CPS);
            AccurateSleep(1000 / (CPS + random));

            offsetx = (rand() % (jitter_strength * 2)) - jitter_strength;
            offsety = (rand() % (jitter_strength * 2)) - jitter_strength;

            SetCursorPos(p.x + offsetx, p.y + offsety);
        }       
    }
}

DWORD WINAPI CPSDropSpikeThread(LPVOID lParam)
{
    int random;
    double temp_cps_drop_CPS, temp_cps_spike_CPS, randomd;
    clock_t cpsdrop_starttime, cpsspike_starttime;
    while (1)
    {
        if (havingFun && isonmc && (!disable_while_inv || (disable_while_inv && !in_inv)))
        {
            if (cps_drops && !currently_incpsdrop && !currently_incpsspike)
            {
                random = rand() % (100 - 1 + 0) + 0;
                if (random < cps_drops_percentage)
                {
                    currently_incpsdrop = true;
                    randomd = ((double)rand() / RAND_MAX) * ((max_BCPS - BCPS) - (min_BCPS - BCPS)) + (min_BCPS - BCPS);
                    cpsdrop_starttime = clock();
                    temp_cps_drop_CPS = -((CPS * cps_drops_val) / 100); //cps_drop_val -X%
                    temp_cps_drop_CPS += (randomd * cps_drops_val) / 100;
                    cps_drop_CPS = temp_cps_drop_CPS;
                }
                else
                    cps_drop_CPS = 0;
            }

            if (cps_drops && currently_incpsdrop)
            {
                if ((clock() - cpsdrop_starttime) > 1200) //1.2s? HOW LONG CPSDROP LASTS - CUSTOMIZE IN FUTURE
                    currently_incpsdrop = false;
            }

            if (cps_spikes && !currently_incpsspike && !currently_incpsdrop)
            {
                random = rand() % (100 - 1 + 0) + 0;
                if (random < cps_spikes_percentage)
                {
                    currently_incpsspike = true;
                    randomd = ((double)rand() / RAND_MAX) * ((max_BCPS - BCPS) - (min_BCPS - BCPS)) + (min_BCPS - BCPS);
                    cpsspike_starttime = clock();
                    temp_cps_spike_CPS = (CPS * cps_spikes_val) / 100; //cps_spikes_val -X%
                    temp_cps_spike_CPS += (randomd * cps_spikes_val) / 100;
                    cps_spike_CPS = temp_cps_spike_CPS;
                }
                else
                    cps_spike_CPS = 0;
            }

            if (cps_spikes && currently_incpsspike)
            {
                if ((clock() - cpsspike_starttime) > 1200) //1.2s? HOW LONG CPSDROP LASTS - CUSTOMIZE IN FUTURE
                    currently_incpsspike = false;
            }
        }
        else
        {
            currently_incpsdrop = false;
            currently_incpsspike = false;
            cps_drop_CPS = 0;
            cps_spike_CPS = 0;
        }

        AccurateSleep(rand() % (4000 - 1 + 500) + 500); //4 - 0.5s
    }

    return 0;
}

DWORD WINAPI lagSwitchThread(LPVOID lParam)
{
    //what happens if they cahange lagswitch value mid lagswitch
    string lagcmd;
    int prev_lagSwitch_val = -1;
    bool reset_lagswitch = false;
    bool reset_lagswitch_pt1 = false;
    while (1)
    {
        //powershell "New-NetQosPolicy -Name RANDOMSTRING -AppPathNameMatchCondition javaw.exe -ThrottleRateActionBitsPerSecond 20kb -PolicyStore ActiveStore"
        //powershell "Remove-NetQosPolicy -Name THATSAMESTRING -PolicyStore Activestore -confirm:$false"
        if (lagSwitch && (lagSwitch_val != prev_lagSwitch_val))
        {
            prev_lagSwitch_val = lagSwitch_val;
            reset_lagswitch = true;
        }

        if ((lagSwitch && !prev_lagswitch) || reset_lagswitch_pt1)
        {
            prev_lagswitch = true;
            //activate lagswitch
            lag_rule_name = random_string(8);
            lagcmd.clear();
            lagcmd.append(AY_OBFUSCATE("powershell \"New-NetQosPolicy -Name ", (char)33 + abs(RAND) % (125 - 32)));
            lagcmd.append(lag_rule_name);
            lagcmd.append(AY_OBFUSCATE(" -AppPathNameMatchCondition javaw.exe -ThrottleRateActionBitsPerSecond ", (char)33 + abs(RAND) % (125 - 32)));
            lagcmd.append(to_string(lagSwitch_val));
            lagcmd.append(AY_OBFUSCATE("kb -PolicyStore ActiveStore\"", (char)33 + abs(RAND) % (125 - 32)));

            system(lagcmd.c_str());

            if (reset_lagswitch)
            {
                reset_lagswitch = !reset_lagswitch;
                reset_lagswitch_pt1 = false;
            }
        }
        else if ((!lagSwitch && prev_lagswitch) || reset_lagswitch)
        {
            if (!reset_lagswitch)
                prev_lagswitch = false;
            else
                reset_lagswitch_pt1 = true;
            //deactivate lagswitch
            lagcmd.clear();
            lagcmd.append(AY_OBFUSCATE("powershell \"Remove-NetQosPolicy -Name ", (char)33 + abs(RAND) % (125 - 32)));
            lagcmd.append(lag_rule_name);
            lagcmd.append(AY_OBFUSCATE(" -PolicyStore Activestore -confirm:$false", (char)33 + abs(RAND) % (125 - 32)));

            system(lagcmd.c_str());
            AccurateSleep(100); //dont activate and deactivate too quickly, gotta give it time
        }

        AccurateSleep(1);
    }
}

DWORD WINAPI manageHotkeysThread(LPVOID lParam)
{
    unsigned char prev_keys[255];
    //char* prev = toggle_btn_value;
    //char* prev = blockhit_btn_value;
    //char* prev2 = destr_btn_value;
    //char* prev3 = jittr_btn_value;
    //char* prev3 = inv_btn_value;
    //char* prev3 = streamer_btn_value;
    while (1)
    {
        AccurateSleep(1); //dont kill cpu

        if (!show_hotkeys_gui)
        {
            AccurateSleep(1000);
            continue;
        }

        if (!(hotkeys1 || hotkeys2 || hotkeys3 || hotkeys4 || hotkeys5 || hotkeys6 || hotkeys7))
        {
            AccurateSleep(600);
            continue;
        }
        if (hotkeys1 || hotkeys2 || hotkeys3 || hotkeys4 || hotkeys5 || hotkeys6 || hotkeys7)
        {
            //needs it idk why
            char* str = AY_OBFUSCATE("Press a key", (char)33 + abs(RAND) % (125 - 32));
            printf("");
            if (hotkeys1)
                strncpy(toggle_btn_value, str, sizeof(toggle_btn_value));
            else if (hotkeys2)
                strncpy(destr_btn_value, str, sizeof(destr_btn_value));
            else if (hotkeys3)
                strncpy(jittr_btn_value, str, sizeof(jittr_btn_value));
            else if (hotkeys4)
                strncpy(streamer_btn_value, str, sizeof(streamer_btn_value));
            else if (hotkeys5)
                strncpy(blockhit_btn_value, str, sizeof(blockhit_btn_value));
            else if (hotkeys6)
                strncpy(inv_btn_value, str, sizeof(inv_btn_value));
            else if (hotkeys7)
                strncpy(lagswitch_btn_value, str, sizeof(lagswitch_btn_value));

            for (int i = 0; i < 255; i++) {
                prev_keys[i] = GetAsyncKeyState(i);
            }
            for (int i = 2; i < 255; i++)
            {
                if (prev_keys[i] != GetAsyncKeyState(i))
                {
                    //esc
                    if (i == 27)
                    {
                        if (hotkeys1)
                        {
                            strncpy(toggle_btn_value, getKeyName(0), sizeof(getKeyName(0)));
                            toggle_key = 0;
                            //toggle_btn_value = prev;
                        }
                        else if(hotkeys2)
                        {
                            strncpy(destr_btn_value, getKeyName(0), sizeof(getKeyName(0)));
                            destr_key = 0;
                            //destr_btn_value = prev2;
                        }
                        else if (hotkeys3)
                        {
                            strncpy(jittr_btn_value, getKeyName(0), sizeof(getKeyName(0)));
                            jittr_key = 0;
                            //jittr_btn_value = prev2;
                        }
                        else if (hotkeys4)
                        {
                            strncpy(streamer_btn_value, getKeyName(0), sizeof(getKeyName(0)));
                            streamer_key = 0;
                            //streamer_btn_value = prev2;
                        }
                        else if (hotkeys5)
                        {
                            strncpy(blockhit_btn_value, getKeyName(0), sizeof(getKeyName(0)));
                            blockhit_key = 0;
                            //blockhit_btn_value = prev2;
                        }
                        else if (hotkeys6)
                        {
                            strncpy(inv_btn_value, getKeyName(0), sizeof(getKeyName(0)));
                            inv_key = 0;
                            //blockhit_btn_value = prev2;
                        }
                        else if (hotkeys7)
                        {
                            strncpy(lagswitch_btn_value, getKeyName(0), sizeof(getKeyName(0)));
                            lagswitch_key = 0;
                            //blockhit_btn_value = prev2;
                        }
                        hotkeys1 = false;
                        hotkeys2 = false;
                        hotkeys3 = false;
                        hotkeys4 = false;
                        hotkeys5 = false;
                        hotkeys6 = false;
                        hotkeys7 = false;
                        break;
                    }
                    else
                    {
                            //get actual key name
                            if (hotkeys1)
                            {
                                strncpy(toggle_btn_value, getKeyName(i), sizeof(getKeyName(i)));
                                AccurateSleep(500);
                                toggle_key = i;
                                //prev = toggle_btn_value;
                            }
                            else if(hotkeys2)
                            {
                                strncpy(destr_btn_value, getKeyName(i), sizeof(getKeyName(i)));
                                AccurateSleep(500);
                                destr_key = i;
                                //prev2 = destr_btn_value;

                            }
                            else if (hotkeys3)
                            {
                                strncpy(jittr_btn_value, getKeyName(i), sizeof(getKeyName(i)));
                                AccurateSleep(500);
                                jittr_key = i;
                                //prev3 = jittr_btn_value;
                            }
                            else if (hotkeys4)
                            {
                                strncpy(streamer_btn_value, getKeyName(i), sizeof(getKeyName(i)));
                                AccurateSleep(500);
                                streamer_key = i;
                                //prev3 = streamer_btn_value;
                            }
                            else if (hotkeys5)
                            {
                                strncpy(blockhit_btn_value, getKeyName(i), sizeof(getKeyName(i)));
                                AccurateSleep(500);
                                blockhit_key = i;
                                //prev3 = blockhit_btn_value;
                            }
                            else if (hotkeys6)
                            {
                                strncpy(inv_btn_value, getKeyName(i), sizeof(getKeyName(i)));
                                AccurateSleep(500);
                                inv_key = i;
                                //prev3 = inv_btn_value;
                            }
                            else if (hotkeys7)
                            {
                                strncpy(lagswitch_btn_value, getKeyName(i), sizeof(getKeyName(i)));
                                AccurateSleep(500);
                                lagswitch_key = i;
                                //prev3 = inv_btn_value;
                            }

                        hotkeys1 = false;
                        hotkeys2 = false;
                        hotkeys3 = false;
                        hotkeys4 = false;
                        hotkeys5 = false;
                        hotkeys6 = false;
                        hotkeys7 = false;
                        break;
                    }
                }
            }
        }
    }
}

DWORD WINAPI authThread(LPVOID lParam)
{
    while (1)
    {
        sec_test = false;
        
        if (SymlinkExists(AY_OBFUSCATE("KsDumper")))
            exit(0);

        checkFunEnabled();
        canHaveFun();
        #ifndef DEBUG
        checkUpdates();
        #endif
        
        assert(sec_test);

        AccurateSleep(30000); //every 30s
    }
}

int stopFun()
{
    HKEY hKey = NULL;

    if (lagSwitch)
    {
        lagSwitch = false;
        AccurateSleep(250); //give it time to stop lagswitch
    }

    freeStuff();

    restartProcesses();

    system_no_output(AY_OBFUSCATE("reg delete \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Compatibility Assistant\\Store \" /f", (char)33 + abs(RAND) % (125 - 32)));
    system_no_output(AY_OBFUSCATE("reg delete \"HKEY_CURRENT_USER\\Software\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\Shell\\MuiCache \" /f", (char)33 + abs(RAND) % (125 - 32)));
    system_no_output(AY_OBFUSCATE("reg delete \"HKEY_CURRENT_USER\\Software\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\Shell\\Bags \" /f", (char)33 + abs(RAND) % (125 - 32)));
    system_no_output(AY_OBFUSCATE("reg delete \"HKEY_CURRENT_USER\\Software\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\Shell\\BagMRU \" /f", (char)33 + abs(RAND) % (125 - 32)));
    system_no_output(AY_OBFUSCATE("reg delete \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\Shell\\Bags \" /f", (char)33 + abs(RAND) % (125 - 32)));
    system_no_output(AY_OBFUSCATE("reg delete \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\Shell\\BagMRU \" /f", (char)33 + abs(RAND) % (125 - 32)));
    system_no_output(AY_OBFUSCATE("reg delete \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Compatibility Assistant\\Store \" /f", (char)33 + abs(RAND) % (125 - 32)));
    system_no_output(AY_OBFUSCATE("reg delete \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Compatibility Assistant\\Persisted \" /f", (char)33 + abs(RAND) % (125 - 32)));
    system_no_output(AY_OBFUSCATE("reg delete \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\ShellNoRoam\\MUICache \" /f\n", (char)33 + abs(RAND) % (125 - 32)));
    system_no_output(AY_OBFUSCATE("reg delete \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ComDlg32\\OpenSavePidlMRU \" /f", (char)33 + abs(RAND) % (125 - 32)));
    system_no_output(AY_OBFUSCATE("reg delete \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ComDlg32\\LastVisitedPidlMRU \" /f", (char)33 + abs(RAND) % (125 - 32)));
    system_no_output(AY_OBFUSCATE("reg delete \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ComDlg32\\LastVisitedPidlMRULegacy \" /f", (char)33 + abs(RAND) % (125 - 32)));
    system_no_output(AY_OBFUSCATE("reg delete \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ComDlg32\\OpenSaveMRU \" /f", (char)33 + abs(RAND) % (125 - 32)));
    system_no_output(AY_OBFUSCATE("reg delete \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\UserAssist \" /f", (char)33 + abs(RAND) % (125 - 32)));

    ::ShowWindow(::GetConsoleWindow(), SW_SHOW);
    SetConsoleTextAttribute(hConsole, 15);
    #ifndef DEBUG
    system(AY_OBFUSCATE("cls", (char)33 + abs(RAND) % (125-32)));
    #endif
    exit(0);
    return 0;
}


int drawGui()
{
    HWND opengl_window;
    GLFWwindow* window;
    GLFWimage window_icon[1];

    // Setup window
    if (!glfwInit())
        return 1;

    const char* glsl_version = AY_OBFUSCATE("#version 130", (char)33 + abs(RAND) % (125-32));
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create window with graphics context
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); //noresize
    window = glfwCreateWindow(970, 543, AY_OBFUSCATE("", (char)33 + abs(RAND) % (125-32)), NULL, NULL);
    // Set window icon
    window_icon[0].pixels = icon_image_pixels;
    window_icon[0].height = icon_image_height;
    window_icon[0].width = icon_image_width;

    /* ADD THIS TO DUMP IMAGE AND ADD TO ARRAY
    char* icon = "C:\\Users\\user\\source\\repos\\example_glfw_opengl3\\icon.png";
    window_icon[0].pixels = SOIL_load_image(icon, &window_icon[0].width, &window_icon[0].height, 0, SOIL_LOAD_RGBA);
    FILE* fp = fopen("C:\\Users\\user\\source\\repos\\example_glfw_opengl3\\img_dump.txt", "w+");
    for (int i = 0; i < 1000000; i++) {
        fprintf(fp, "%d, ", (unsigned int)window_icon[0].pixels[i]);
    }
    fclose(fp);*/
    glfwSetWindowIcon(window, 1, window_icon);
    //SOIL_free_image_data(window_icon->pixels);

    /* WAV TO FILE
    unsigned char* buffer = 0;
    ifstream infile("C:\\Users\\Usuario\\source\\repos\\pecesClicker\\sounds\\toggle.wav", ios::binary);
    infile.seekg(0, ios::end);   // get length of file
    int length = infile.tellg();
    buffer = new unsigned char[length];    // allocate memory
    infile.seekg(0, ios::beg);   // position to start of file
    infile.read((char*)buffer, length);  // read entire file
    infile.close();

    FILE* fp = fopen("C:\\Users\\Usuario\\source\\repos\\pecesClicker\\sound_dump.txt", "w+");
    for (int i = 0; i < length; i++) {
        fprintf(fp, "%d, ", (unsigned int)buffer[i]);
    }
    fclose(fp);
    */
    
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync (fps)

    gl3wInit();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.IniFilename = NULL;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Main loop
    opengl_window = GetActiveWindow();
    while (!destruct && !glfwWindowShouldClose(window))
    {
        if (onlyinmc)
        {
            char wnd_title1[256];
            char wnd_title2[256];
            HWND mc_window = FindWindowA(AY_OBFUSCATE("LWJGL", (char)33 + abs(RAND) % (125 - 32)), nullptr);
            active_window = GetForegroundWindow();
            GetWindowText(active_window, wnd_title1, sizeof(wnd_title1));
            GetWindowText(mc_window, wnd_title2, sizeof(wnd_title2));
            isonmc = !strcmp(wnd_title1, wnd_title2);
        }
        else
            isonmc = true;

        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (chroma)
            clear_color = handleChroma(clear_color);

        if (!hide && streamermode)
        {
            hide = true;
            ::ShowWindow(opengl_window, SW_HIDE);
        }
        else if (hide && !streamermode)
        {
            hide = false;
            ::ShowWindow(opengl_window, SW_SHOW);
        }
        char* buf;
        stringstream strs;
        strs << jitter_strength;
        string temp_str = strs.str();
        buf = (char*)temp_str.c_str();

        ImGuiWindowFlags window_flags = 0;

        // Background window BEGIN
        {
            window_flags = 0;
            window_flags |= ImGuiWindowFlags_NoResize;
            window_flags |= ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoTitleBar;
            window_flags |= ImGuiWindowFlags_NoSavedSettings;
            window_flags |= ImGuiWindowFlags_NoScrollbar;
            window_flags |= ImGuiWindowFlags_NoCollapse;
            window_flags |= ImGuiWindowFlags_NoBackground;
            window_flags |= ImGuiWindowFlags_NoScrollWithMouse;


            ImGui::SetNextWindowPos(ImVec2(-8, -8));
            ImGui::SetNextWindowSize(ImVec2(1000, 600));
            ImGui::Begin(AY_OBFUSCATE("Background window", (char)33 + abs(RAND) % (125 - 32)), NULL, window_flags);

            if (custombackground)
            {
                //keep track of prev and current, if not equals then change
                if (strcmp(prev_bg, custombackgroundbuf) != 0)
                {
                    strcpy(prev_bg, custombackgroundbuf);
                    LoadTextureFromFile(custombackgroundbuf, &bg_image_texture, &bg_image_width, &bg_image_height);
                }
                ImGui::Image((void*)(intptr_t)bg_image_texture, ImVec2(bg_image_width, bg_image_height));
            }
        }
        // Background window END

        // GUI Start

        ImGui::SetNextWindowPos(ImVec2((io.DisplaySize.x * 0.5f) + 60, io.DisplaySize.y * 0.56f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * 0.6f, io.DisplaySize.y * 0.9f));
        ImGui::BeginChild(AY_OBFUSCATE("Peces Clicker", (char)33 + abs(RAND) % (125 - 32)), ImVec2(io.DisplaySize.x * 0.6f, io.DisplaySize.y * 0.9f), false, window_flags);

        ImGui::Text(AY_OBFUSCATE("Peces:", (char)33 + abs(RAND) % (125 - 32)));

        if(!humanized_clicks)
        { 
            prev_maxCPS = max_CPS;
            prev_minCPS = min_CPS;
            ImGui::InputFloat(AY_OBFUSCATE("Min CPS", (char)33 + abs(RAND) % (125 - 32)), &min_CPS, 0.5f, 1.0f, AY_OBFUSCATE("%.1f", (char)33 + abs(RAND) % (125 - 32)));
            ImGui::InputFloat(AY_OBFUSCATE("Max CPS", (char)33 + abs(RAND) % (125 - 32)), &max_CPS, 0.5f, 1.0f, AY_OBFUSCATE("%.1f", (char)33 + abs(RAND) % (125 - 32)));

            min_CPS = min_CPS < 0 ? 0 : min_CPS;
            max_CPS = max_CPS < 0 ? 0 : max_CPS;
            if (max_CPS < min_CPS)
            {
                min_CPS = prev_minCPS;
                max_CPS = prev_maxCPS;
            }
        }

        CPS = (max_CPS + min_CPS) / 2;

        if (blockhit)
        {
            prev_maxBCPS = max_BCPS;
            prev_minBCPS = min_BCPS;
            ImGui::InputFloat(AY_OBFUSCATE("Blockhit Min CPS", (char)33 + abs(RAND) % (125 - 32)), &min_BCPS, 0.5f, 1.0f, AY_OBFUSCATE("%.1f", (char)33 + abs(RAND) % (125 - 32)));
            ImGui::InputFloat(AY_OBFUSCATE("Blockhit Max CPS", (char)33 + abs(RAND) % (125 - 32)), &max_BCPS, 0.5f, 1.0f, AY_OBFUSCATE("%.1f", (char)33 + abs(RAND) % (125 - 32)));

            min_BCPS = min_BCPS < 0 ? 0 : min_BCPS;
            max_BCPS = max_BCPS < 0 ? 0 : max_BCPS;
            if (max_BCPS < min_BCPS)
            {
                min_BCPS = prev_minBCPS;
                max_BCPS = prev_maxBCPS;
            }
        }

        BCPS = (max_BCPS + min_BCPS) / 2;

        if (multipleclick)
        {
            ImGui::InputFloat(AY_OBFUSCATE("Multiple-Click Value", (char)33 + abs(RAND) % (125 - 32)), &multipleclick_count, 0.05f, 0.1f, AY_OBFUSCATE("%.2f", (char)33 + abs(RAND) % (125 - 32)));
            if (multipleclick_count > 1.0f)
                multipleclick_count = 1.0f;
            else if (multipleclick_count < 0.0f)
                multipleclick_count = 0.0f;

            havingFun = false;
        }

        if (jitter)
        {
            ImGui::InputInt(AY_OBFUSCATE("Jitter Strength", (char)33 + abs(RAND) % (125 - 32)), &jitter_strength);
            if (jitter_strength < 0)
                jitter_strength = 0;
        }

        if (inc_increase)
        {
            ImGui::InputFloat(AY_OBFUSCATE("Incremental Delay (s)", (char)33 + abs(RAND) % (125 - 32)), &inc_increase_val, 0.1f, 0.3f, AY_OBFUSCATE("%.1f", (char)33 + abs(RAND) % (125 - 32)));
            if (inc_increase_val < 0)
                inc_increase_val = 0;
        }

        if (exhaust)
        {
            ImGui::InputFloat(AY_OBFUSCATE("Exhaust Timer (s)", (char)33 + abs(RAND) % (125 - 32)), &exhaust_timer, 0.5f, 1.0f, AY_OBFUSCATE("%.1f", (char)33 + abs(RAND) % (125 - 32)));
            if (exhaust_timer < 0)
                exhaust_timer = 0;

            ImGui::InputFloat(AY_OBFUSCATE("Exhaust Strength (s)", (char)33 + abs(RAND) % (125 - 32)), &exhaust_val, 0.5f, 1.0f, AY_OBFUSCATE("%.1f", (char)33 + abs(RAND) % (125 - 32)));
            if (exhaust_val < 0)
                exhaust_val = 0;
        }

        if (cps_drops)
        {
            ImGui::InputInt(AY_OBFUSCATE("CPS Drop Chance %", (char)33 + abs(RAND) % (125 - 32)), &cps_drops_percentage);
            if (cps_drops_percentage < 0)
                cps_drops_percentage = 0;
            else if (cps_drops_percentage > 100)
                cps_drops_percentage = 100;

            ImGui::InputInt(AY_OBFUSCATE("CPS Drop Value %", (char)33 + abs(RAND) % (125 - 32)), &cps_drops_val);
            if (cps_drops_val < 0)
                cps_drops_val = 0;
            else if (cps_drops_val > 100)
                cps_drops_val = 100;

        }

        if (cps_spikes)
        {
            ImGui::InputInt(AY_OBFUSCATE("CPS Spike Chance %", (char)33 + abs(RAND) % (125 - 32)), &cps_spikes_percentage);
            if (cps_spikes_percentage < 0)
                cps_spikes_percentage = 0;
            else if (cps_spikes_percentage > 100)
                cps_spikes_percentage = 100;

            ImGui::InputInt(AY_OBFUSCATE("CPS Spike Value %", (char)33 + abs(RAND) % (125 - 32)), &cps_spikes_val);
            if (cps_spikes_val < 0)
                cps_spikes_val = 0;
            else if (cps_spikes_val > 100)
                cps_spikes_val = 100;

        }

        if (clicksound)
        {
            ImGui::Combo(AY_OBFUSCATE("Click Sound", (char)33 + abs(RAND) % (125 - 32)), &click_type_selected, click_types, IM_ARRAYSIZE(click_types));
            if (click_type_selected == IM_ARRAYSIZE(click_types) - 1)
            {
                ImGui::InputText(AY_OBFUSCATE("Path to .wav File", (char)33 + abs(RAND) % (125 - 32)), customclick, IM_ARRAYSIZE(customclick));
            }
        }

        if (lagSwitch)
        {
            ImGui::InputInt(AY_OBFUSCATE("Lagswitch throttle (kb)", (char)33 + abs(RAND) % (125 - 32)), &lagSwitch_val);
            if (lagSwitch_val < 0)
                lagSwitch_val = 0;
        }

        ImGui::Checkbox(AY_OBFUSCATE("Jitter", (char)33 + abs(RAND) % (125 - 32)), &jitter);
        ImGui::SameLine();
        //ImGui::Dummy(ImVec2(30.0f, 0.0f));
        //ImGui::SameLine();
        ImGui::Checkbox(AY_OBFUSCATE("Only in MC", (char)33 + abs(RAND) % (125 - 32)), &onlyinmc);
        //ImGui::SameLine();
        //ImGui::Dummy(ImVec2(30.0f, 0.0f));
        ImGui::SameLine();
        ImGui::Checkbox(AY_OBFUSCATE("No click-down", (char)33 + abs(RAND) % (125 - 32)), &no_clickdown);
        //ImGui::SameLine();
        //ImGui::Dummy(ImVec2(30.0f, 0.0f));
        ImGui::SameLine();
        ImGui::Checkbox(AY_OBFUSCATE("Click sound", (char)33 + abs(RAND) % (125 - 32)), &clicksound);

        ImGui::Checkbox(AY_OBFUSCATE("Incremental Inc.", (char)33 + abs(RAND) % (125 - 32)), &inc_increase);
        ImGui::SameLine();
        ImGui::Checkbox(AY_OBFUSCATE("Blockhit", (char)33 + abs(RAND) % (125 - 32)), &blockhit);
        ImGui::SameLine();
        ImGui::Checkbox(AY_OBFUSCATE("Hide", (char)33 + abs(RAND) % (125 - 32)), &streamermode);
        ImGui::SameLine();
        ImGui::Checkbox(AY_OBFUSCATE("Humanized Clicks", (char)33 + abs(RAND) % (125 - 32)), &humanized_clicks);

        ImGui::Checkbox(AY_OBFUSCATE("Exhaust ", (char)33 + abs(RAND) % (125 - 32)), &exhaust);
        //ImGui::SameLine();
        //ImGui::Dummy(ImVec2(11.0f, 0.0f));
        ImGui::SameLine();
        ImGui::Checkbox(AY_OBFUSCATE("CPS Drops", (char)33 + abs(RAND) % (125 - 32)), &cps_drops);
        //ImGui::SameLine();
        //ImGui::Dummy(ImVec2(0.1f, 0.0f));
        ImGui::SameLine();
        ImGui::Checkbox(AY_OBFUSCATE("CPS Spikes ", (char)33 + abs(RAND) % (125 - 32)), &cps_spikes);
        if (running_as_admin)
        {
           // ImGui::SameLine();
            //ImGui::Dummy(ImVec2(0.1f, 0.0f));
            ImGui::SameLine();
            ImGui::Checkbox(AY_OBFUSCATE("Lagswitch", (char)33 + abs(RAND) % (125 - 32)), &lagSwitch);
        }

        ImGui::Checkbox(AY_OBFUSCATE("Multiple-click ", (char)33 + abs(RAND) % (125 - 32)), &multipleclick);
        ImGui::SameLine();
        ImGui::Checkbox(AY_OBFUSCATE("No LClick Blockhit", (char)33 + abs(RAND) % (125 - 32)), &no_left_click_blockhit);
        ImGui::SameLine();
        ImGui::Checkbox(AY_OBFUSCATE("Disable in Inv", (char)33 + abs(RAND) % (125 - 32)), &disable_while_inv);

        ImGui::Dummy(ImVec2(0.0f, 7.0f));

        if (havingFun)
        {
            if (ImGui::Button(AY_OBFUSCATE("Deactivate", (char)33 + abs(RAND) % (125 - 32))))
            {
                havingFun = !havingFun;
                if (sound_on_activate)
                    PlaySound(toggle_sound, NULL, SND_MEMORY | SND_ASYNC);
                if (invert_on_toggle)
                    invertColor();
            }
        }
        else
        {
            if (ImGui::Button(AY_OBFUSCATE(" Activate ", (char)33 + abs(RAND) % (125 - 32))))
            {
                havingFun = !havingFun;
                if (sound_on_activate)
                    PlaySound(toggle_sound, NULL, SND_MEMORY | SND_ASYNC);
                if (invert_on_toggle)
                    invertColor();
            }
                
        }

        ImGui::SameLine();
        ImGui::Dummy(ImVec2(33.0f, 0.0f));

        ImGui::SameLine();
        if (ImGui::Button(AY_OBFUSCATE("Save Settings", (char)33 + abs(RAND) % (125 - 32))))
            saveSettings();

        ImGui::SameLine();
        ImGui::Dummy(ImVec2(33.0f, 0.0f));

        ImGui::SameLine();
        if (ImGui::Button(AY_OBFUSCATE("Self-Destruct", (char)33 + abs(RAND) % (125 - 32))))
        {
            destruct = true;
            stopFun();
        }

        ImGui::Dummy(ImVec2(115.0f, 0.0f));
        ImGui::SameLine();
        ImGui::Text(AY_OBFUSCATE("Randomness = %.1f", (char)33 + abs(RAND) % (125 - 32)), ((double)rand() / RAND_MAX) * ((max_CPS - CPS) - (min_CPS - CPS)) + (min_CPS - CPS));

        ImGui::Dummy(ImVec2(115.0f, 0.0f));
        ImGui::SameLine();
        ImGui::Text(AY_OBFUSCATE("Real CPS = %.1f", (char)33 + abs(RAND) % (125 - 32)), realcps);

        float avg = 0.0f;
        for (int i = 0; i < IM_ARRAYSIZE(cpsvalues); i++)
        {
            avg += cpsvalues[i];
        }

        avg /= IM_ARRAYSIZE(cpsvalues);
        char overlay[32];
        sprintf(overlay, AY_OBFUSCATE("avg %.2f", (char)33 + abs(RAND) % (125 - 32)), avg);

        if (prev_realcps != 0)
            ImGui::PlotLines("", cpsvalues, IM_ARRAYSIZE(cpsvalues), 0, overlay, min_CPS * 0.85, max_CPS * 1.15, ImVec2(0, 50));
        else
            ImGui::PlotLines("", cpsvalues, IM_ARRAYSIZE(cpsvalues), 0, overlay, -1, 1, ImVec2(0, 50));
        
        if (realcps != 0)
        {
            //move left
            rotate(begin(cpsvalues), begin(cpsvalues) + 1, end(cpsvalues));

            cpsvalues[IM_ARRAYSIZE(cpsvalues) - 1] = (float) prev_realcps;
        }
        else
            fill(begin(cpsvalues), end(cpsvalues), 0);

        prev_realcps = realcps;
            
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        if(ImGui::Button(AY_OBFUSCATE("Change hotkeys", (char)33 + abs(RAND) % (125-32))))
            show_hotkeys_gui = !show_hotkeys_gui;
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(73.0f, 0.0f));
        ImGui::SameLine();
        if(ImGui::Button(AY_OBFUSCATE("Misc", (char)33 + abs(RAND) % (125-32))))
            show_color_config = !show_color_config;
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(74.0f, 0.0f));
        ImGui::SameLine();
        if(ImGui::Button(AY_OBFUSCATE("Presets", (char)33 + abs(RAND) % (125-32))))
            show_presets_gui = !show_presets_gui;

        /*if (clicksound && click_type_selected != IM_ARRAYSIZE(click_types) - 1)
        {
            if (ImGui::Button(AY_OBFUSCATE(" Volume UP ", (char)33 + abs(RAND) % (125 - 32))))
                changeVolume(click_type_selected, VOLUME_UP, 1);

            if (ImGui::Button(AY_OBFUSCATE(" Volume DOWN ", (char)33 + abs(RAND) % (125 - 32))))
                changeVolume(click_type_selected, VOLUME_DOWN, 1);
        }*/

        ImGui::EndChild();

        // GUI End
            
        // Info GUI
            {
                ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.35, 5));
                ImGui::BeginChild(AY_OBFUSCATE("Version Window", (char)33 + abs(RAND) % (125 - 32)), ImVec2(0, 50), false, window_flags);

                ImGui::Text(AY_OBFUSCATE("Peces v%s - Made by Baitinq", (char)33 + abs(RAND) % (125-32)), VERSION);
                ImGui::Text(AY_OBFUSCATE("    Licensed to: %s", (char)33 + abs(RAND) % (125-32)), license_reciever);
            
                ImGui::EndChild();
            }
        // Info GUI End

        if (show_color_config)
        {
            ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.59, 5));
            ImGui::BeginChild(AY_OBFUSCATE("Color Configuration", (char)33 + abs(RAND) % (125 - 32)), ImVec2(460, 70), false, window_flags);


            ImGui::ColorEdit3(AY_OBFUSCATE("Color", (char)33 + abs(RAND) % (125-32)), (float*)&clear_color); // Edit 3 floats representing a color

            ImGui::InputText(AY_OBFUSCATE("Img path", (char)33 + abs(RAND) % (125 - 32)), custombackgroundbuf, IM_ARRAYSIZE(custombackgroundbuf));

            ImGui::Checkbox(AY_OBFUSCATE("Chroma", (char)33 + abs(RAND) % (125-32)), &chroma);
            ImGui::SameLine();
            ImGui::Checkbox(AY_OBFUSCATE("Custom bg", (char)33 + abs(RAND) % (125 - 32)), &custombackground);
            if (custombackground && (custombackgroundbuf[0] == 0))
              custombackground = false;
            ImGui::SameLine();
            ImGui::Checkbox(AY_OBFUSCATE("Toggle sound", (char)33 + abs(RAND) % (125 - 32)), &sound_on_activate);
            ImGui::SameLine();
            ImGui::Checkbox(AY_OBFUSCATE("Invert toggle", (char)33 + abs(RAND) % (125 - 32)), &invert_on_toggle);

            ImGui::EndChild();
        }

        if (show_presets_gui)
        {
            ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 70, io.DisplaySize.y - 80));
            ImGui::BeginChild(AY_OBFUSCATE("Preset Selection", (char)33 + abs(RAND) % (125 - 32)), ImVec2(0, 100), false, window_flags);

            if (ImGui::Button(AY_OBFUSCATE("Default", (char)33 + abs(RAND) % (125-32))))
                setDefault();

            if (ImGui::Button(AY_OBFUSCATE("Legit", (char)33 + abs(RAND) % (125-32))))
                setLegit();

            if (ImGui::Button(AY_OBFUSCATE("Tryhard", (char)33 + abs(RAND) % (125-32))))
                setTryhard();


            ImGui::EndChild();
        }

        if (show_hotkeys_gui)
        {
            ImGui::SetNextWindowPos(ImVec2(10, 10));
            ImGui::BeginChild(AY_OBFUSCATE("Hotkey Configuration", (char)33 + abs(RAND) % (125 - 32)), ImVec2(210, 160), false, window_flags);

            ImGui::Text(AY_OBFUSCATE("Toggle Key", (char)33 + abs(RAND) % (125-32)));
            ImGui::SameLine();
            ImGui::PushItemWidth(ImGui::GetWindowSize().x * 0.5f);
            if(ImGui::Button(toggle_btn_value))
                hotkeys1 = true;
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip(AY_OBFUSCATE("Press ESC to set to None.", (char)33 + abs(RAND) % (125-32)));

            ImGui::Text(AY_OBFUSCATE("Blockhit Key", (char)33 + abs(RAND) % (125-32)));
            ImGui::SameLine();
            ImGui::PushItemWidth(ImGui::GetWindowSize().x * 0.5f);
            if (ImGui::Button(blockhit_btn_value))
                hotkeys5 = true;
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip(AY_OBFUSCATE("Press ESC to set to None.", (char)33 + abs(RAND) % (125-32)));

            ImGui::Text(AY_OBFUSCATE("Streamer Key", (char)33 + abs(RAND) % (125-32)));
            ImGui::SameLine();
            ImGui::PushItemWidth(ImGui::GetWindowSize().x * 0.5f);
            if (ImGui::Button(streamer_btn_value))
                hotkeys4 = true;
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip(AY_OBFUSCATE("Press ESC to set to None.", (char)33 + abs(RAND) % (125-32)));

            ImGui::Text(AY_OBFUSCATE("Jitter Key", (char)33 + abs(RAND) % (125-32)));
            ImGui::SameLine();
            ImGui::PushItemWidth(ImGui::GetWindowSize().x * 0.5f);
            if (ImGui::Button(jittr_btn_value))
                hotkeys3 = true;
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip(AY_OBFUSCATE("Press ESC to set to None.", (char)33 + abs(RAND) % (125-32)));

            ImGui::Text(AY_OBFUSCATE("Inventory Key", (char)33 + abs(RAND) % (125 - 32)));
            ImGui::SameLine();
            ImGui::PushItemWidth(ImGui::GetWindowSize().x * 0.5f);
            if (ImGui::Button(inv_btn_value))
                hotkeys6 = true;
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip(AY_OBFUSCATE("Press ESC to set to None.", (char)33 + abs(RAND) % (125 - 32)));

            if(running_as_admin)
            { 
                ImGui::Text(AY_OBFUSCATE("Lagswitch Key", (char)33 + abs(RAND) % (125 - 32)));
                ImGui::SameLine();
                ImGui::PushItemWidth(ImGui::GetWindowSize().x * 0.5f);
                if (ImGui::Button(lagswitch_btn_value))
                    hotkeys7 = true;
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip(AY_OBFUSCATE("Press ESC to set to None.", (char)33 + abs(RAND) % (125 - 32)));
            }

            ImGui::Text(AY_OBFUSCATE("Self-Destruct Key", (char)33 + abs(RAND) % (125-32)));
            ImGui::SameLine();
            ImGui::PushItemWidth(ImGui::GetWindowSize().x * 0.5f);
            if (ImGui::Button(destr_btn_value))
                hotkeys2 = true;
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip(AY_OBFUSCATE("Press ESC to set to None.", (char)33 + abs(RAND) % (125-32)));

            ImGui::EndChild();
        }

        ImGui::End();
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

int saveSettings()
{
    string json;
    json.append(AY_OBFUSCATE("{", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"min_CPS\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(min_CPS));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"max_CPS\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(max_CPS));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"min_BCPS\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(min_BCPS));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"max_BCPS\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(max_BCPS));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"blockhit\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(blockhit));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"jitterclick\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(jitter));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"jitter_strength\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(jitter_strength));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"no-click-down\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(no_clickdown));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"clicksound\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(clicksound));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"clicksound_type\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(click_type_selected));
            if (strlen(customclick) > 2)
            {
                json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
                json.append(AY_OBFUSCATE("\"custom_clicksound_path\":", (char)33 + abs(RAND) % (125 - 32)));  json.append("\""); json.append(customclick); json.append("\"");
            }
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
                json.append(AY_OBFUSCATE("\"humanized_clicks\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(humanized_clicks));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"incremental_increase\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(inc_increase));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"incremental_increase_delay\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(inc_increase_val));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"streamer_mode\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(streamermode));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"exhaust\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(exhaust));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"exhaust_timer\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(exhaust_timer));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"exhaust_strength\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(exhaust_val));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"CPS_drops\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(cps_drops));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"CPS_drop_chance\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(cps_drops_percentage));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"CPS_drop_value\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(cps_drops_val));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"CPS_spikes_chance\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(cps_spikes_percentage));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"CPS_spikes_value\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(cps_spikes_val));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"lagswitch\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(lagSwitch));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"lagswitch_throttle\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(lagSwitch_val));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"onlyinmc\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(onlyinmc));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"sound_on_activate\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(sound_on_activate));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"invert_on_toggle\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(invert_on_toggle));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"multipleclick\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(multipleclick));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"multipleclick_value\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(multipleclick_count));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"no_lclick_blockhit\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(no_left_click_blockhit));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"disable_in_inventory\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(disable_while_inv));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"chroma\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(chroma));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"custom_background\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(custombackground));
            if(strlen(custombackgroundbuf) > 2)
            { 
                json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
                json.append(AY_OBFUSCATE("\"custom_background_path\":", (char)33 + abs(RAND) % (125 - 32)));  json.append("\""); json.append(custombackgroundbuf); json.append("\"");
            }
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"color_x\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(clear_color.x));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"color_y\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(clear_color.y));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"color_z\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(clear_color.z));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"toggle_key\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(toggle_key));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"blockhit_key\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(blockhit_key));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"streamer_key\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(streamer_key));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"jitter_key\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(jittr_key));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"inventory_key\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(inv_key));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"lagswitch_key\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(lagswitch_key));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"selfdestruct_key\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(destr_key));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"show_hotkeys_gui\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(show_hotkeys_gui));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"show_colors_gui\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(show_color_config));
        json.append(AY_OBFUSCATE(",", (char)33 + abs(RAND) % (125 - 32)));
            json.append(AY_OBFUSCATE("\"show_presets_gui\":", (char)33 + abs(RAND) % (125 - 32)));  json.append(to_string(show_presets_gui));
    json.append(AY_OBFUSCATE("}", (char)33 + abs(RAND) % (125 - 32)));

    
    std::replace(json.begin(), json.end(), '\\', '/'); // fixes path bug

    //printf("json:\n %s\n", json.c_str());
    //printf("encoded: %s\n", encode((unsigned char*)json.c_str(), strlen(json.c_str())).c_str());

    /* save settings to file */
    WCHAR path[MAX_PATH];
    char dir[_MAX_DIR];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    wstring ws(path);
    string filepath(ws.begin(), ws.end());
    _splitpath(filepath.c_str(), NULL, dir, NULL, NULL);
    string actualpath = "C:" + string(dir) + settingsname;
    //printf("writing to file: %s\n", actualpath.c_str());

    ofstream out(actualpath.c_str());
    out << encode((unsigned char*) encode((unsigned char*)json.c_str(), strlen(json.c_str())).c_str(), strlen(encode((unsigned char*)json.c_str(), strlen(json.c_str())).c_str())).c_str();
    out.close();

    return 0;
}

int loadSettings()
{
    WCHAR path[MAX_PATH];
    char dir[_MAX_DIR];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    wstring ws(path);
    string filepath(ws.begin(), ws.end());
    _splitpath(filepath.c_str(), NULL, dir, NULL, NULL);
    string actualpath = "C:" + string(dir) + settingsname;

    ifstream settings_file(actualpath.c_str());

    if (settings_file.fail())
        return 0;

    string json((istreambuf_iterator<char>(settings_file)), istreambuf_iterator<char>());
    string decodedjson = decode(decode(json));
    //printf("read from file:\n %s\n", json.c_str());

    rapidjson::Document d;
    d.Parse(decodedjson.c_str());

    /* set variables */
    if(d.HasMember(AY_OBFUSCATE("min_CPS", (char)33 + abs(RAND) % (125 - 32))))
    { 
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("min_CPS", (char)33 + abs(RAND) % (125 - 32))];
        min_CPS = read.GetDouble();
    }    
    if (d.HasMember(AY_OBFUSCATE("max_CPS", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("max_CPS", (char)33 + abs(RAND) % (125 - 32))];
        max_CPS = read.GetDouble();
    }
    if (d.HasMember(AY_OBFUSCATE("min_BCPS", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("min_BCPS", (char)33 + abs(RAND) % (125 - 32))];
        min_BCPS = read.GetDouble();
    }
    if (d.HasMember(AY_OBFUSCATE("max_BCPS", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("max_BCPS", (char)33 + abs(RAND) % (125 - 32))];
        max_BCPS = read.GetDouble();
    }
    if (d.HasMember(AY_OBFUSCATE("blockhit", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("blockhit", (char)33 + abs(RAND) % (125 - 32))];
        blockhit = read.GetInt();
    }
    if (d.HasMember(AY_OBFUSCATE("jitterclick", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("jitterclick", (char)33 + abs(RAND) % (125 - 32))];
        jitter = read.GetInt();
    }
    if (d.HasMember(AY_OBFUSCATE("jitter_strength", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("jitter_strength", (char)33 + abs(RAND) % (125 - 32))];
        jitter_strength = read.GetInt();
    }
    if (d.HasMember(AY_OBFUSCATE("no-click-down", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("no-click-down", (char)33 + abs(RAND) % (125 - 32))];
        no_clickdown = read.GetInt();
    }
    if (d.HasMember(AY_OBFUSCATE("clicksound", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("clicksound", (char)33 + abs(RAND) % (125 - 32))];
        clicksound = read.GetInt();
    }
    if (d.HasMember(AY_OBFUSCATE("clicksound_type", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("clicksound_type", (char)33 + abs(RAND) % (125 - 32))];
        click_type_selected = read.GetInt();
    }
    if (d.HasMember(AY_OBFUSCATE("custom_clicksound_path", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("custom_clicksound_path", (char)33 + abs(RAND) % (125 - 32))];
        strncpy(customclick, read.GetString(), sizeof(customclick));
    }
    if (d.HasMember(AY_OBFUSCATE("humanized_clicks", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("humanized_clicks", (char)33 + abs(RAND) % (125 - 32))];
        humanized_clicks = read.GetInt();
    }
    if (d.HasMember(AY_OBFUSCATE("incremental_increase", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("incremental_increase", (char)33 + abs(RAND) % (125 - 32))];
        inc_increase = read.GetInt();
    }
    if (d.HasMember(AY_OBFUSCATE("incremental_increase_delay", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("incremental_increase_delay", (char)33 + abs(RAND) % (125 - 32))];
        inc_increase_val = read.GetDouble();
    }
    if (d.HasMember(AY_OBFUSCATE("streamer_mode", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("streamer_mode", (char)33 + abs(RAND) % (125 - 32))];
        streamermode = read.GetInt();
    }
    if (d.HasMember(AY_OBFUSCATE("exhaust", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("exhaust", (char)33 + abs(RAND) % (125 - 32))];
        exhaust = read.GetInt();
    }
    if (d.HasMember(AY_OBFUSCATE("exhaust_timer", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("exhaust_timer", (char)33 + abs(RAND) % (125 - 32))];
        exhaust_timer = read.GetDouble();
    }
    if (d.HasMember(AY_OBFUSCATE("exhaust_strength", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("exhaust_strength", (char)33 + abs(RAND) % (125 - 32))];
        exhaust_val = read.GetDouble();
    }
    if (d.HasMember(AY_OBFUSCATE("CPS_drops", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("CPS_drops", (char)33 + abs(RAND) % (125 - 32))];
        cps_drops = read.GetInt();
    }
    if (d.HasMember(AY_OBFUSCATE("CPS_drop_chance", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("CPS_drop_chance", (char)33 + abs(RAND) % (125 - 32))];
        cps_drops_percentage = read.GetInt();
    }
    if (d.HasMember(AY_OBFUSCATE("CPS_drop_value", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("CPS_drop_value", (char)33 + abs(RAND) % (125 - 32))];
        cps_drops_val = read.GetInt();
    }
    if (d.HasMember(AY_OBFUSCATE("CPS_spikes_chance", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("CPS_spikes_chance", (char)33 + abs(RAND) % (125 - 32))];
        cps_spikes_percentage = read.GetInt();
    }
    if (d.HasMember(AY_OBFUSCATE("CPS_spikes_value", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("CPS_spikes_value", (char)33 + abs(RAND) % (125 - 32))];
        cps_spikes_val = read.GetInt();
    }
    if (d.HasMember(AY_OBFUSCATE("lagswitch", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("lagswitch", (char)33 + abs(RAND) % (125 - 32))];
        lagSwitch = read.GetInt();
    }
    if (d.HasMember(AY_OBFUSCATE("lagswitch_throttle", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("lagswitch_throttle", (char)33 + abs(RAND) % (125 - 32))];
        lagSwitch_val = read.GetInt();
    }
    if (d.HasMember(AY_OBFUSCATE("onlyinmc", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("onlyinmc", (char)33 + abs(RAND) % (125 - 32))];
        onlyinmc = read.GetInt();
    }
    if (d.HasMember(AY_OBFUSCATE("sound_on_activate", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("sound_on_activate", (char)33 + abs(RAND) % (125 - 32))];
        sound_on_activate = read.GetInt();
    }
    if (d.HasMember(AY_OBFUSCATE("invert_on_toggle", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("invert_on_toggle", (char)33 + abs(RAND) % (125 - 32))];
        invert_on_toggle = read.GetInt();
    }
    if (d.HasMember(AY_OBFUSCATE("multipleclick", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("multipleclick", (char)33 + abs(RAND) % (125 - 32))];
        multipleclick = read.GetInt();
    }
    if (d.HasMember(AY_OBFUSCATE("multipleclick_value", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("multipleclick_value", (char)33 + abs(RAND) % (125 - 32))];
        multipleclick_count = read.GetDouble();
    }
    if (d.HasMember(AY_OBFUSCATE("no_lclick_blockhit", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("no_lclick_blockhit", (char)33 + abs(RAND) % (125 - 32))];
        no_left_click_blockhit = read.GetInt();
    }
    if (d.HasMember(AY_OBFUSCATE("disable_in_inventory", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("disable_in_inventory", (char)33 + abs(RAND) % (125 - 32))];
        disable_while_inv = read.GetInt();
    }
    if (d.HasMember(AY_OBFUSCATE("chroma", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("chroma", (char)33 + abs(RAND) % (125 - 32))];
        chroma = read.GetInt();
    }
    if (d.HasMember(AY_OBFUSCATE("custom_background", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("custom_background", (char)33 + abs(RAND) % (125 - 32))];
        custombackground = read.GetInt();
    }
    if (d.HasMember(AY_OBFUSCATE("custom_background_path", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("custom_background_path", (char)33 + abs(RAND) % (125 - 32))];
        strncpy(custombackgroundbuf, read.GetString(), sizeof(custombackgroundbuf));
    }
    if (d.HasMember(AY_OBFUSCATE("color_x", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("color_x", (char)33 + abs(RAND) % (125 - 32))];
        clear_color.x = read.GetFloat();
    }
    if (d.HasMember(AY_OBFUSCATE("color_y", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("color_y", (char)33 + abs(RAND) % (125 - 32))];
        clear_color.y = read.GetFloat();
    }
    if (d.HasMember(AY_OBFUSCATE("color_z", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("color_z", (char)33 + abs(RAND) % (125 - 32))];
        clear_color.z = read.GetFloat();
    }
    if (d.HasMember(AY_OBFUSCATE("toggle_key", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("toggle_key", (char)33 + abs(RAND) % (125 - 32))];
        toggle_key = read.GetInt();
    }
    if (d.HasMember(AY_OBFUSCATE("blockhit_key", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("blockhit_key", (char)33 + abs(RAND) % (125 - 32))];
        blockhit_key = read.GetInt();
    }
    if (d.HasMember(AY_OBFUSCATE("streamer_key", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("streamer_key", (char)33 + abs(RAND) % (125 - 32))];
        streamer_key = read.GetInt();
    }
    if (d.HasMember(AY_OBFUSCATE("jitter_key", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("jitter_key", (char)33 + abs(RAND) % (125 - 32))];
        jittr_key = read.GetInt();
    }
    if (d.HasMember(AY_OBFUSCATE("inventory_key", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("inventory_key", (char)33 + abs(RAND) % (125 - 32))];
        inv_key = read.GetInt();
    }
    if (d.HasMember(AY_OBFUSCATE("lagswitch_key", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("lagswitch_key", (char)33 + abs(RAND) % (125 - 32))];
        lagswitch_key = read.GetInt();
    }
    if (d.HasMember(AY_OBFUSCATE("selfdestruct_key", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("selfdestruct_key", (char)33 + abs(RAND) % (125 - 32))];
        destr_key = read.GetInt();
    }
    if (d.HasMember(AY_OBFUSCATE("show_hotkeys_gui", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("show_hotkeys_gui", (char)33 + abs(RAND) % (125 - 32))];
        show_hotkeys_gui = read.GetInt();
    }
    if (d.HasMember(AY_OBFUSCATE("show_colors_gui", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("show_colors_gui", (char)33 + abs(RAND) % (125 - 32))];
        show_color_config = read.GetInt();
    }
    if (d.HasMember(AY_OBFUSCATE("show_presets_gui", (char)33 + abs(RAND) % (125 - 32))))
    {
        rapidjson::Value& read = d[(char*)AY_OBFUSCATE("show_presets_gui", (char)33 + abs(RAND) % (125 - 32))];
        show_presets_gui = read.GetInt();
    }
 
    return 0;
}

ImVec4 handleChroma(ImVec4 color)
{
    if (up_x)
    {
        if (color.x + 0.003f <= 0.999f)
            color.x += 0.003f;
        else
            up_x = false;
    }
    else
    {
        if (color.x - 0.003f >= 0.001f)
            color.x -= 0.003f;
        else
            up_x = true;
    }

    if (up_y)
    {
        if (color.y + 0.003f <= 0.999f)
            color.y += 0.003f;
        else
            up_y = false;
    }
    else
    {
        if (color.y - 0.003f >= 0.001f)
            color.y -= 0.003f;
        else
            up_y = true;
    }

    if (up_z)
    {
        if (color.z + 0.003f <= 0.999f)
            color.z += 0.003f;
        else
            up_z = false;
    }
    else
    {
        if (color.z - 0.003f >= 0.001f)
            color.z -= 0.003f;
        else
            up_z = true;
    }

    color.w = 1.00f;

    return color;
}

int getPassword()
{
    CURL* curl;
    string buffer;

    curl = curl_easy_init();
    if (!curl)
        stopFun();

    /* SET UP URL */
    string url = AY_OBFUSCATE("/api/license_pass.php", (char)33 + abs(RAND) % (125-32));
    url.append(AY_OBFUSCATE("?", (char)33 + abs(RAND) % (125-32)));
    url.append(decode(getHwId().c_str()).c_str());
    url.insert(0, AY_OBFUSCATE(WEB_ROOT, (char)33 + abs(RAND) % (125-32)));

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
    if (curl_easy_perform(curl) != CURLE_OK)
        stopFun();
    curl_easy_cleanup(curl);
    
    if (strstr(buffer.c_str(), AY_OBFUSCATE("xddd1", (char)33 + abs(RAND) % (125-32))))
    {
        SecureZeroMemory((void*)buffer.c_str(), sizeof(buffer.c_str()));
        printf(AY_OBFUSCATE("Not verified. (Error Code: %s&*%s)\n", (char)33 + abs(RAND) % (125-32)), getHwId().c_str(), encode((const unsigned char*)getHwId().c_str(), 23).erase(20, 17).c_str());
        AccurateSleep(1000);
        stopFun();
    }

    strcpy(key, buffer.c_str());
    
    SecureZeroMemory((void*)buffer.c_str(), sizeof(buffer.c_str()));
   
    return 0;
}

int getLicenseReciever()
{
    CURL* curl;
    string buffer;

    curl = curl_easy_init();
    if (!curl)
        stopFun();

    /* SET UP URL */
    string url = AY_OBFUSCATE("/api/license_name.php", (char)33 + abs(RAND) % (125-32));
    url.append(AY_OBFUSCATE("?", (char)33 + abs(RAND) % (125-32)));
    url.append(decode(getHwId().c_str()).c_str());
    url.insert(0, AY_OBFUSCATE(WEB_ROOT, (char)33 + abs(RAND) % (125-32)));

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
    if (curl_easy_perform(curl) != CURLE_OK)
        stopFun();
    curl_easy_cleanup(curl);

    if (strstr(buffer.c_str(), AY_OBFUSCATE("xddd1", (char)33 + abs(RAND) % (125-32))))
    {
        SecureZeroMemory((void*)buffer.c_str(), sizeof(buffer.c_str()));
        printf(AY_OBFUSCATE("Not verified. (Error Code: %s&*%s)\n", (char)33 + abs(RAND) % (125-32)), getHwId().c_str(), encode((const unsigned char*)getHwId().c_str(), 23).erase(20, 17).c_str());
        AccurateSleep(1000);
        stopFun();
    }

    strcpy(license_reciever, buffer.c_str());

    SecureZeroMemory((void*)buffer.c_str(), sizeof(buffer.c_str()));

    return 0;
}

int getCmdName()
{
    CURL* curl;
    string buffer;

    curl = curl_easy_init();
    if (!curl)
    {
        freeStuff();
        exit(0);
    }

    /* SET UP URL */
    string url = AY_OBFUSCATE("/api/license_cmd.php", (char)33 + abs(RAND) % (125-32));
    url.append(AY_OBFUSCATE("?", (char)33 + abs(RAND) % (125-32)));
    url.append(decode(getHwId().c_str()).c_str());
    url.insert(0, AY_OBFUSCATE(WEB_ROOT, (char)33 + abs(RAND) % (125-32)));

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
    if (curl_easy_perform(curl) != CURLE_OK)
    {
        freeStuff();
        exit(0);
    }

    curl_easy_cleanup(curl);

    if (strstr(buffer.c_str(), AY_OBFUSCATE("xddd1", (char)33 + abs(RAND) % (125-32))))
    {
        SecureZeroMemory((void*)buffer.c_str(), sizeof(buffer.c_str()));
        printf(AY_OBFUSCATE("Not verified. (Error Code: %s&*%s)\n", (char)33 + abs(RAND) % (125-32)), getHwId().c_str(), encode((const unsigned char*)getHwId().c_str(), 23).erase(20, 17).c_str());
        AccurateSleep(1000);
        freeStuff();
        exit(0);
    }

    strcpy(cmdname, buffer.c_str());

    SecureZeroMemory((void*)buffer.c_str(), sizeof(buffer.c_str()));

    return 0;
}

int getSettingsFile()
{
    CURL* curl;
    string buffer;

    curl = curl_easy_init();
    if (!curl)
        stopFun();

    /* SET UP URL */
    string url = AY_OBFUSCATE("/api/settings_name.php", (char)33 + abs(RAND) % (125 - 32));
    url.append(AY_OBFUSCATE("?", (char)33 + abs(RAND) % (125 - 32)));
    url.append(decode(getHwId().c_str()).c_str());
    url.insert(0, AY_OBFUSCATE(WEB_ROOT, (char)33 + abs(RAND) % (125 - 32)));

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
    if (curl_easy_perform(curl) != CURLE_OK)
        stopFun();
    curl_easy_cleanup(curl);

    if (strstr(buffer.c_str(), AY_OBFUSCATE("xddd1", (char)33 + abs(RAND) % (125 - 32))))
    {
        SecureZeroMemory((void*)buffer.c_str(), sizeof(buffer.c_str()));
        printf(AY_OBFUSCATE("Not verified. (Error Code: %s&*%s)\n", (char)33 + abs(RAND) % (125 - 32)), getHwId().c_str(), encode((const unsigned char*)getHwId().c_str(), 23).erase(20, 17).c_str());
        AccurateSleep(1000);
        exit(0);
    }

    strcpy(settingsname, buffer.c_str());

    SecureZeroMemory((void*)buffer.c_str(), sizeof(buffer.c_str()));

    return 0;
}

int canHaveFun()
{
    CURL* curl;
    string buffer;

    curl = curl_easy_init();
    if (!curl)
        stopFun();

    /* SET UP URL */
    string url = AY_OBFUSCATE("/api/auth.php", (char)33 + abs(RAND) % (125-32));
    url.append(AY_OBFUSCATE("?", (char)33 + abs(RAND) % (125-32)));
    url.append(decode(getHwId().c_str()).c_str());
    url.insert(0, AY_OBFUSCATE(WEB_ROOT, (char)33 + abs(RAND) % (125-32)));

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
    if (curl_easy_perform(curl) != CURLE_OK)
        stopFun();
    curl_easy_cleanup(curl);

    printf(AY_OBFUSCATE("Verifying identity...\n", (char)33 + abs(RAND) % (125-32)));
    AccurateSleep(1000);


    if (!strstr(buffer.c_str(), decode(getHwId().c_str()).c_str()))
    {
        SecureZeroMemory((void*)buffer.c_str(), sizeof(buffer.c_str()));
        printf(AY_OBFUSCATE("Not verified. (Error Code: %s&*%s)\n", (char)33 + abs(RAND) % (125-32)), getHwId().c_str(), encode((const unsigned char*)getHwId().c_str(), 23).erase(20, 17).c_str());
        AccurateSleep(1000);
        stopFun();
    }
    SecureZeroMemory((void*)buffer.c_str(), sizeof(buffer.c_str()));

    printf(AY_OBFUSCATE("Succesful authentication.\n", (char)33 + abs(RAND) % (125-32)));
    AccurateSleep(1000);

    sec_test = true;

    return 0;
}

int checkFunEnabled()
{
    CURL* curl;
    string buffer;

    curl = curl_easy_init();
    if (!curl)
        stopFun();

    /* SET UP URL */
    string url = AY_OBFUSCATE("/api/oops.php", (char)33 + abs(RAND) % (125-32));
    url.insert(0, AY_OBFUSCATE(WEB_ROOT, (char)33 + abs(RAND) % (125-32)));

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
    if (curl_easy_perform(curl) != CURLE_OK)
        stopFun();
    curl_easy_cleanup(curl);

    if (!strstr(buffer.c_str(), kswitch))
    {
        SecureZeroMemory((void*)buffer.c_str(), sizeof(buffer.c_str()));
        stopFun();
    }
    SecureZeroMemory((void*)buffer.c_str(), sizeof(buffer.c_str()));

    sec_test = true;

    return 0;
}

int checkUpdates()
{
    CURL* curl;
    string buffer;

    curl = curl_easy_init();
    if (!curl)
        stopFun();

    /* SET UP URL */
    string url = AY_OBFUSCATE("/api/version.php", (char)33 + abs(RAND) % (125-32));
    url.insert(0, AY_OBFUSCATE(WEB_ROOT, (char)33 + abs(RAND) % (125-32)));

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
    if (curl_easy_perform(curl) != CURLE_OK)
        stopFun();
    curl_easy_cleanup(curl);

    printf(AY_OBFUSCATE("\nChecking for updates...\n", (char)33 + abs(RAND) % (125-32)));
    AccurateSleep(1000);

    if (!strstr(buffer.c_str(), VERSION))
    {
        printf(AY_OBFUSCATE("Update found. Please update to the newest version (v%s).\n", (char)33 + abs(RAND) % (125-32)), buffer.c_str());
        SecureZeroMemory((void*)buffer.c_str(), sizeof(buffer.c_str()));
        AccurateSleep(1000);
        stopFun();
    }
    SecureZeroMemory((void*)buffer.c_str(), sizeof(buffer.c_str()));

    printf(AY_OBFUSCATE("No updates found.\n", (char)33 + abs(RAND) % (125-32)));
    AccurateSleep(1000);

    return 0;
}

int changeVolume(int chosen_sound, int option, int cycles)
{
    int size = -1;
    char* sound = NULL;
    switch (chosen_sound)
    {
    case 0:
        sound = jitterclick_click_sound;
        size = IM_ARRAYSIZE(jitterclick_click_sound);
        break;
    case 1:
        sound = butterfly_click_sound;
        size = IM_ARRAYSIZE(butterfly_click_sound);
        break;
    case 2:
        sound = low_cps_click_sound;
        size = IM_ARRAYSIZE(low_cps_click_sound);
        break;
    case 3:
        sound = g303_click_sound;
        size = IM_ARRAYSIZE(g303_click_sound);
        break;
    case 4:
        sound = g502_click_sound;
        size = IM_ARRAYSIZE(g502_click_sound);
        break;
    case 5:
        sound = gpro_click_sound;
        size = IM_ARRAYSIZE(gpro_click_sound);
        break;
    default:
        return 1;
    }

   // printf("size of the sound is %d\n", size);

    for (int j = 0; j < cycles; j++)
    {
        if (option == VOLUME_UP)
        {
           // printf("increasing volume...\n");
            for (int i = 242; i < size; i++)
            {
                sound[i] = sound[i] != 0 ? sound[i] + 1 : 0;
                //jitterclick_click_sound[i] = jitterclick_click_sound[i] != 0 ? jitterclick_click_sound[i] + 1 : 0;
                
               // printf("%d, ", sound[i]);
            }
           // printf("done increasing.\n");
        }
        else if (option == VOLUME_DOWN)
        {
           // printf("decreasing volume...\n");
            for (int i = 242; i < size; i++)//42 - try an use the other sounds directly to see if it still sounds weird
            {
                sound[i] = sound[i] != 0 ? sound[i] - 1 : 0;
                //sound[i] * 0.95;
            }
           // printf("done decreasing.\n");
        }
    }
   
    return 0;
}

int invertColor()
{
    clear_color.x = 1.0f - clear_color.x;
    clear_color.y = 1.0f - clear_color.y;
    clear_color.z = 1.0f - clear_color.z;

    return 0;
}

char* getKeyName(int key)
{
    char result[1024];

    if (key == 0)
        strncpy(result, AY_OBFUSCATE("None", (char)33 + abs(RAND) % (125 - 32)), sizeof(result));
    else
    {
        WCHAR name[1024];
        ZeroMemory(result, sizeof(result));
        ZeroMemory(name, sizeof(name));
        UINT scanCode = MapVirtualKeyW(key, MAPVK_VK_TO_VSC);
        LONG lParamValue = (scanCode << 16);
        GetKeyNameTextW(lParamValue, name, 1024);
        sprintf(result, AY_OBFUSCATE("%ws", (char)33 + abs(RAND) % (125 - 32)), name);
    }

    return result;
}

bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
       return false;

    // Resize to 970x543
    int new_w = 970;
    int new_h = 543;
    unsigned char* resized_image_data = (unsigned char*) malloc(new_w * new_h * 4);
    stbir_resize_uint8(image_data, image_width, image_height, 0, resized_image_data, new_w, new_h, 0, 4);
    stbi_image_free(image_data);

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload pixels into texture
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, new_w, new_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, resized_image_data);
    stbi_image_free(resized_image_data);

    *out_texture = image_texture;
    *out_width = new_w;
    *out_height = new_h;

    return true;
}

bool IsProcessRunning(const char* processName)
{
    bool exists = false;
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (Process32First(snapshot, &entry))
        while (Process32Next(snapshot, &entry))
            if (!stricmp(entry.szExeFile, processName))
                exists = true;

    CloseHandle(snapshot);
    return exists;
}

bool SymlinkExists(const char* symlink)
{
    HANDLE hDevice = INVALID_HANDLE_VALUE;
    std::string path = AY_OBFUSCATE("\\\\.\\");
    path.append(symlink);
    std::wstring stemp = std::wstring(path.begin(), path.end());

    hDevice = CreateFileW(
        stemp.c_str(),
        0,
        //GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (INVALID_HANDLE_VALUE == hDevice)
    {
        CloseHandle(hDevice);
        return false;
    }

    CloseHandle(hDevice);
    return true;
}

string getHwId()
{
    int Id_num = 0;
    DWORD HddNumber = 0;
    string HddId;
    string CPUId;
    string Id;

    int cpuinfo[4] = { 0, 0, 0, 0 };
    __cpuid(cpuinfo, 0);

    for (int i = 0; i < 3; i++)
    {
        Id_num *= 10;
        Id_num += cpuinfo[i];
    }

    Id_num += 124;
    Id_num *= 2;

    CPUId = to_string(Id_num);
    GetVolumeInformation(AY_OBFUSCATE("C://", (char)33 + abs(RAND) % (125-32)), NULL, NULL, &HddNumber, NULL, NULL, NULL, NULL);
    HddId = to_string(HddNumber);
    //remove last two characters
    HddId.pop_back();
    HddId.pop_back();

    Id = CPUId + HddId;

    //returns 26 character long string
    return encode((const unsigned char*)Id.c_str(), 15);
}

string encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
    string ret;
    string base64_chars(b64_c);

    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (in_len--) {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (i = 0; (i < 4); i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i)
    {
        for (j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; (j < i + 1); j++)
            ret += base64_chars[char_array_4[j]];

        while ((i++ < 3))
            ret += '=';
    }

    return ret;
}

string decode(std::string const& encoded_string) {
    string ret;
    string base64_chars(b64_c);

    int in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];

    while (in_len-- && (encoded_string[in_] != '=')) {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if (i == 4) {
            for (i = 0; i < 4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++)
                ret += char_array_3[i];
            i = 0;
        }
    }

    if (i) {
        for (j = 0; j < i; j++)
            char_array_4[j] = base64_chars.find(char_array_4[j]);

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);

        for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
    }

    return ret;
}

int gettimeofday(struct timeval* tp, struct timezone* tzp)
{
    // Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
    // This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
    // until 00:00:00 January 1, 1970 
    static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

    SYSTEMTIME  system_time;
    FILETIME    file_time;
    uint64_t    time;

    GetSystemTime(&system_time);
    SystemTimeToFileTime(&system_time, &file_time);
    time = ((uint64_t)file_time.dwLowDateTime);
    time += ((uint64_t)file_time.dwHighDateTime) << 32;

    tp->tv_sec = (long)((time - EPOCH) / 10000000L);
    tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
    return 0;
}

bool IsElevated()
{
    bool fRet = false;
    HANDLE hToken = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION Elevation;
        DWORD cbSize = sizeof(TOKEN_ELEVATION);
        if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
            fRet = Elevation.TokenIsElevated;
        }
    }
    if (hToken) {
        CloseHandle(hToken);
    }
    return fRet;
}

string random_string(size_t length)
{
    auto randchar = []() -> char
    {
        const size_t max_index = (sizeof(b64_c) - 1);
        return b64_c[rand() % max_index];
    };
    string str(length, 0);
    generate_n(str.begin(), length, randchar);
    return str;
}

int OnlyNumbersTextFilter(ImGuiTextEditCallbackData* data)
{
    if (!(data->EventChar >= '0' && data->EventChar <= '9')) 
        return 1;
    return 0;
}

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

PVOID GetPEB()
{
    return (PVOID)__readgsqword(0x0C * sizeof(PVOID));
}

int CheckNtGlobalFlag()
{
    PVOID pPeb = GetPEB();
    PVOID pPeb64 = 0;
    DWORD offsetNtGlobalFlag = 0;

    offsetNtGlobalFlag = 0xBC;
    DWORD NtGlobalFlag = *(PDWORD)((PBYTE)pPeb + offsetNtGlobalFlag);
    // is in debug mode
    if ((NtGlobalFlag & (0x10 | 0x20 | 0x40)) || IsDebuggerPresent())
        exit(0);

    return 0;
}

int freeStuff()
{
    // First fill with zeroes and then random
    SecureZeroMemory(VERSION, sizeof(VERSION));
    SecureZeroMemory(license_reciever, sizeof(license_reciever));
    SecureZeroMemory(cmdname, sizeof(cmdname));
    SecureZeroMemory(key, sizeof(key));
    SecureZeroMemory(click_types, sizeof(click_types));
    SecureZeroMemory(toggle_btn_value, sizeof(toggle_btn_value));
    SecureZeroMemory(blockhit_btn_value, sizeof(blockhit_btn_value));
    SecureZeroMemory(jittr_btn_value, sizeof(jittr_btn_value));
    SecureZeroMemory(streamer_btn_value, sizeof(streamer_btn_value));
    SecureZeroMemory(destr_btn_value, sizeof(destr_btn_value));
    SecureZeroMemory(b64_c, sizeof(b64_c));
    SecureZeroMemory(low_cps_click_sound, sizeof(low_cps_click_sound));
    SecureZeroMemory(butterfly_click_sound, sizeof(butterfly_click_sound));
    SecureZeroMemory(jitterclick_click_sound, sizeof(jitterclick_click_sound));
    SecureZeroMemory(icon_image_pixels, sizeof(icon_image_pixels));
    SecureZeroMemory(customclick, sizeof(customclick));
    SecureZeroMemory(custombackgroundbuf, sizeof(custombackgroundbuf));
    SecureZeroMemory(cpsvalues, sizeof(cpsvalues));

    generate_n(VERSION, sizeof(VERSION), std::rand);
    generate_n(license_reciever, sizeof(license_reciever), std::rand);
    generate_n(cmdname, sizeof(cmdname), std::rand);
    generate_n(key, sizeof(key), std::rand);
    memcpy(click_types, (void*)memcpy, sizeof(click_types)); //generate_n doesnt work for array
    generate_n(toggle_btn_value, sizeof(toggle_btn_value), std::rand);
    generate_n(blockhit_btn_value, sizeof(blockhit_btn_value), std::rand);
    generate_n(jittr_btn_value, sizeof(jittr_btn_value), std::rand);
    generate_n(streamer_btn_value, sizeof(streamer_btn_value), std::rand);
    generate_n(b64_c, sizeof(b64_c), std::rand);
    generate_n(destr_btn_value, sizeof(destr_btn_value), std::rand);
    generate_n(low_cps_click_sound, sizeof(low_cps_click_sound), std::rand);
    generate_n(butterfly_click_sound, sizeof(butterfly_click_sound), std::rand);
    generate_n(jitterclick_click_sound, sizeof(jitterclick_click_sound), std::rand);
    generate_n(icon_image_pixels, sizeof(icon_image_pixels), std::rand);
    generate_n(customclick, sizeof(customclick), std::rand);
    generate_n(custombackgroundbuf, sizeof(custombackgroundbuf), std::rand);
    memcpy(cpsvalues, (void*)memcpy, sizeof(cpsvalues)); //generate_n doesnt work for array

    return 0;
}

int restartProcesses()
{
    if (running_as_admin)
    {
        SHELLEXECUTEINFO si = { sizeof(si) };
        si.lpVerb = TEXT(AY_OBFUSCATE("runas", (char)33 + abs(RAND) % (125 - 32)));
        si.lpFile = TEXT(AY_OBFUSCATE("cmd", (char)33 + abs(RAND) % (125 - 32)));
        si.lpParameters = TEXT(AY_OBFUSCATE("/c \"net stop DPS & sc start DPS & net stop pcasvc & net start pcasvc & net stop diagtrack & net start diagtrack & net stop dnscache & net start dnscache\"", (char)33 + abs(RAND) % (125 - 32)));
        si.hwnd = NULL;
        si.nShow = SW_HIDE;
        ShellExecuteEx(&si);
    }

    return 0;
}

int system_no_output(const char* cmd)
{
    string command = cmd;
    command.insert(0, AY_OBFUSCATE("/C "));

    SHELLEXECUTEINFOA ShExecInfo = { 0 };
    ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    ShExecInfo.hwnd = NULL;
    ShExecInfo.lpVerb = NULL;
    ShExecInfo.lpFile = AY_OBFUSCATE("cmd.exe");
    ShExecInfo.lpParameters = command.c_str();
    ShExecInfo.lpDirectory = NULL;
    ShExecInfo.nShow = SW_HIDE;
    ShExecInfo.hInstApp = NULL;

    ShellExecuteExA(&ShExecInfo);
        
    return 0;
}

int setDefault()
{
    min_CPS = 12.5;
    max_CPS = 15.0;
    jitter_strength = 3;
    inc_increase_val = 0.7f;

    inc_increase = false;

    return 0;
}

int setLegit()
{
    min_CPS = 9.0;
    max_CPS = 14.0;
    jitter_strength = 3;
    inc_increase_val = 1.0f;

    inc_increase = true;

    return 0;
}

int setTryhard()
{
    min_CPS = 12.2;
    max_CPS = 16.0;
    min_BCPS = 4.5;
    max_BCPS = 7.5;
    jitter_strength = 3;
    inc_increase_val = 1.0f;

    inc_increase = true;
    blockhit = true;

    return 0;
}
