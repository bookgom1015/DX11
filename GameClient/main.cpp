#include "pch.h"

#include "Engine.h"

#include "Entity.h"
#include "Asset.h"
#include "AMesh.h"


// 260129 목요일 과제
// 1. 태양계 만들어보기
//   - 가운데 태양 오브젝트 띄우기
//   - 태양 주변을 공전하는 행성들 만들어 보기

// 2. 태양 및 주변 행성들에 빌보드 효과 적용하기
//  - 빌보드 효과 : 항상 카메라를 마주보는 회전 상태로 만들기


// const 보다 더 엄격한 상수 키워드
// constexpr 

// 변수 템플릿
template<typename T>
bool g_IsTrue = true;

// 템플릿 특수화
// T1, T2 가 다르게 설정되면, 초기값을 100으로
// T1, T2 타입이 같은 타입으로 설정되면 초기값을 200 으로 설정한 전역변수를 만든다.
template<typename T1, typename T2>
int NewData = 100;
template<typename T1>
int NewData<T1, T1> = 200;

// if constexpr () 


void Test()
{   
 
    int k = 200;

    // i 는 상수화된 변수, 딱 한번만 초기화 가능
    const int i = k;    

    // j 는 더 엄격한 상수, 컴파일 타임에 어떤 숫자가 들어갈 것인지 확신할 수 있어야 함
    // j 의 초기값으로 입력되는 k 는 변수기 때문에, 컴파일 시점에는 어떤 숫자로 j 가 
    // 초기화 될지 알 수 없음
    //constexpr int j = k;

    g_IsTrue<float> = 0.f;
    int a = NewData<int, float>;
    int b = NewData<char, char>;

    int num = 0;
    // if 구문 옆에 constexpr 를 붙이면, if 조건안에 반드시 컴파일 타임에 확신할 수 있는 
    // 참 or 거짓이 들어와야만 한다. 컴파일 시간안에 확실할 수 있다면, 해당 조건구문만 
    // 남기고 다른 코드는 전부 삭제(컴파일 최적화) 가능
    if constexpr (0 == 10)
    {

    }
    else
    {

    }
}




HINSTANCE hInst;

INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// SAL : 주석 언어
int APIENTRY wWinMain(_In_      HINSTANCE hInstance,
                     _In_opt_   HINSTANCE hPrevInstance,
                     _In_       LPWSTR    lpCmdLine,
                     _In_       int       nCmdShow)
{         
    Test();


    // CRT new, delete, 디버깅 모드에서 메모리 누수 추적, 출력창에 알림
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    // 누수 발생지점 중단점 걸어주는 기능
    //_CrtSetBreakAlloc(203); 

    hInst = hInstance;

    // Engine 초기화
    // 최상위 관리자
    if (FAILED(Engine::GetInst()->Init(hInstance, 1600, 900)))
    {
        return 0;
    }
    
    // 메세지 루프
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GAMECLIENT));
    MSG msg = {};

    while (true)
    {
        // 메세지 큐에서 메세지를 꺼낸게 있다.
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

            // 단축키 관련된 내용이면 TranslateAccelerator 함수에서 처리
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                // 단축키 관련된 이벤트가 아니면 TranslateMessage, DispatchMessage 함수를 이용해서 처리
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        // 메세지 큐에 메세지가 없었다.
        else
        {
            // Game 실행, 1 프레임
            if (FAILED(Engine::GetInst()->Progress()))
                break;
        } 
    }

    return (int) msg.wParam;
}


#include "KeyMgr.h"

// 프로시저 함수
// 윈도우에 발생한 사건(이벤트, 메세지)를 처리해주는 함수
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_MOUSEWHEEL:
    {
        KeyMgr::GetInst()->SetMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam));
    }
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
