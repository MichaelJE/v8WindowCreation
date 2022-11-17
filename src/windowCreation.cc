#ifndef UNICODE
#define UNICODE
#endif 

#include <node.h>
#include <iostream>
#include <windows.h>

using namespace v8;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);



            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));

            EndPaint(hwnd, &ps);
        }
        return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

class WindowCreation {
  public:
    explicit WindowCreation(Isolate* isolate) {
      node::AddEnvironmentCleanupHook(isolate, DeleteInstance, this);
    }

    static void DeleteInstance(void* data) {
      delete static_cast<WindowCreation*>(data);
    }


  int buildWin()
  {
    // Register the window class.
    const wchar_t CLASS_NAME[]  = L"Sample Window Class";
    std::cout << "Calling function...";
    WNDCLASS wc = { };

    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = GetModuleHandle(nullptr);
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window.
    std::cout << "creating window...";
    HWND hwnd = CreateWindowEx(
      0,                              // Optional window styles.
      CLASS_NAME,                     // Window class
      L"Coding Windows",    // Window text
      WS_OVERLAPPEDWINDOW,            // Window style

      // Size and position
      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

      NULL,       // Parent window    
      NULL,       // Menu
      GetModuleHandle(nullptr),  // Instance handle
      NULL        // Additional application data
    );

    // if (hwnd == NULL)
    // {
    //     return 0;
    // }
    std::cout << "Showing window...";
    ShowWindow(hwnd, 1);

    // Run the message loop.

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
  }

};

static void Method(const v8::FunctionCallbackInfo<v8::Value>& info) {
  WindowCreation* data = reinterpret_cast<WindowCreation*>(info.Data().As<External>()->Value());

  info.GetReturnValue().Set((double)data->buildWin());
}

NODE_MODULE_INIT() {
  std::cout << "Creating isolation...";
  Isolate* isolate = context->GetIsolate();

  std::cout << "Creating window creating...";
  WindowCreation* data = new WindowCreation(isolate);

  std::cout << "Creating external...";
  Local<External> external = External::New(isolate, data);

  std::cout << "Creating context...";
  exports->Set(context,
               String::NewFromUtf8(isolate, "method").ToLocalChecked(),
               FunctionTemplate::New(isolate, Method, external)
                  ->GetFunction(context).ToLocalChecked()).FromJust();
}