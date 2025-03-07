#include <Windows.h>

// D3D ��뿡 �ʿ��� ���̺귯������ ��ũ�մϴ�.
#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

// D3D ��뿡 �ʿ��� ������ϵ��� �����մϴ�.
#include <d3d11.h>
#include <d3dcompiler.h>

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

#include "json.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <format>

#include <vector>
#include "Console.h"
#include "jsonExample.h"

// 1. Define the triangle vertices
struct FVertexSimple
{
    float x, y, z;    // Position
    float r, g, b, a; // Color
};

// Structure for a 3D vector
struct FVector3
{
    float x, y, z;
    FVector3(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}
};

// �ﰢ���� �ϵ� �ڵ�
FVertexSimple triangle_vertices[] =
{
    {  0.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f }, // Top vertex (red)
    {  1.0f, -1.0f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f }, // Bottom-right vertex (green)
    { -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f, 1.0f }  // Bottom-left vertex (blue)
};

FVertexSimple cube_vertices[] =
{
    // Front face (Z+)
    { -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f }, // Bottom-left (red)
    { -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, 1.0f }, // Top-left (yellow)
    {  0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 1.0f }, // Bottom-right (green)
    { -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, 1.0f }, // Top-left (yellow)
    {  0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 1.0f }, // Top-right (blue)
    {  0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 1.0f }, // Bottom-right (green)

    // Back face (Z-)
    { -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, 1.0f }, // Bottom-left (cyan)
    {  0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f, 1.0f }, // Bottom-right (magenta)
    { -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f, 1.0f }, // Top-left (blue)
    { -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f, 1.0f }, // Top-left (blue)
    {  0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f, 1.0f }, // Bottom-right (magenta)
    {  0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 1.0f }, // Top-right (yellow)

    // Left face (X-)
    { -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f, 1.0f }, // Bottom-left (purple)
    { -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f, 1.0f }, // Top-left (blue)
    { -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 1.0f }, // Bottom-right (green)
    { -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f, 1.0f }, // Top-left (blue)
    { -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, 1.0f }, // Top-right (yellow)
    { -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 1.0f }, // Bottom-right (green)

    // Right face (X+)
    {  0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.0f, 1.0f }, // Bottom-left (orange)
    {  0.5f, -0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 1.0f }, // Bottom-right (gray)
    {  0.5f,  0.5f, -0.5f,  0.5f, 0.0f, 0.5f, 1.0f }, // Top-left (purple)
    {  0.5f,  0.5f, -0.5f,  0.5f, 0.0f, 0.5f, 1.0f }, // Top-left (purple)
    {  0.5f, -0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 1.0f }, // Bottom-right (gray)
    {  0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.5f, 1.0f }, // Top-right (dark blue)

    // Top face (Y+)
    { -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.5f, 1.0f }, // Bottom-left (light green)
    { -0.5f,  0.5f,  0.5f,  0.0f, 0.5f, 1.0f, 1.0f }, // Top-left (cyan)
    {  0.5f,  0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f }, // Bottom-right (white)
    { -0.5f,  0.5f,  0.5f,  0.0f, 0.5f, 1.0f, 1.0f }, // Top-left (cyan)
    {  0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 0.0f, 1.0f }, // Top-right (brown)
    {  0.5f,  0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f }, // Bottom-right (white)

    // Bottom face (Y-)
    { -0.5f, -0.5f, -0.5f,  0.5f, 0.5f, 0.0f, 1.0f }, // Bottom-left (brown)
    { -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f }, // Top-left (red)
    {  0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.5f, 1.0f }, // Bottom-right (purple)
    { -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f }, // Top-left (red)
    {  0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 1.0f }, // Top-right (green)
    {  0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.5f, 1.0f }, // Bottom-right (purple)
};

#include "Sphere.h"



class URenderer
{
public:
    // Direct3D 11 ��ġ(Device)�� ��ġ ���ؽ�Ʈ(Device Context) �� ���� ü��(Swap Chain)�� �����ϱ� ���� �����͵�
    ID3D11Device* Device = nullptr; // GPU�� ����ϱ� ���� Direct3D ��ġ
    ID3D11DeviceContext* DeviceContext = nullptr; // GPU ��� ������ ����ϴ� ���ؽ�Ʈ
    IDXGISwapChain* SwapChain = nullptr; // ������ ���۸� ��ü�ϴ� �� ���Ǵ� ���� ü��

    // �������� �ʿ��� ���ҽ� �� ���¸� �����ϱ� ���� ������
    ID3D11Texture2D* FrameBuffer = nullptr; // ȭ�� ��¿� �ؽ�ó
    ID3D11RenderTargetView* FrameBufferRTV = nullptr; // �ؽ�ó�� ���� Ÿ������ ����ϴ� ��
    ID3D11RasterizerState* RasterizerState = nullptr; // �����Ͷ����� ����(�ø�, ä��� ��� �� ����)
    ID3D11Buffer* ConstantBuffer = nullptr; // ���̴��� �����͸� �����ϱ� ���� ��� ����

    FLOAT ClearColor[4] = { 0.025f, 0.025f, 0.025f, 1.0f }; // ȭ���� �ʱ�ȭ(clear)�� �� ����� ���� (RGBA)
    D3D11_VIEWPORT ViewportInfo; // ������ ������ �����ϴ� ����Ʈ ����

public:
    // ������ �ʱ�ȭ �Լ�
    void Create(HWND hWindow)
    {
        // Direct3D ��ġ �� ���� ü�� ����
        CreateDeviceAndSwapChain(hWindow);

        // ������ ���� ����
        CreateFrameBuffer();

        // �����Ͷ����� ���� ����
        CreateRasterizerState();

        // ���� ���ٽ� ���� �� ����� ���´� �� �ڵ忡���� �ٷ��� ����
    }

    // Direct3D ��ġ �� ���� ü���� �����ϴ� �Լ�
    void CreateDeviceAndSwapChain(HWND hWindow)
    {
        // �����ϴ� Direct3D ��� ������ ����
        D3D_FEATURE_LEVEL featurelevels[] = { D3D_FEATURE_LEVEL_11_0 };

        // ���� ü�� ���� ����ü �ʱ�ȭ
        DXGI_SWAP_CHAIN_DESC swapchaindesc;
        ZeroMemory(&swapchaindesc, sizeof(DXGI_SWAP_CHAIN_DESC));

        // ���� ü�� ����
        swapchaindesc.BufferDesc.Width = 0;   // â ũ�� ����
        swapchaindesc.BufferDesc.Height = 0;  // â ũ�� ����
        swapchaindesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;  // ���� ����

        // **���� �߰��� �κ�: RefreshRate ����**
        swapchaindesc.BufferDesc.RefreshRate.Numerator = 60;  // 60Hz
        swapchaindesc.BufferDesc.RefreshRate.Denominator = 1;

        swapchaindesc.SampleDesc.Count = 1;  // ��Ƽ ���ø� ��Ȱ��ȭ
        swapchaindesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // ���� Ÿ������ ���
        swapchaindesc.BufferCount = 2;  // ���� ���۸�
        swapchaindesc.OutputWindow = hWindow;  // �������� â �ڵ�
        swapchaindesc.Windowed = TRUE;  // â ���
        swapchaindesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;  // ���� ���
        swapchaindesc.Flags = 0;

        // Direct3D ��ġ�� ���� ü���� ����
        HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
            D3D11_CREATE_DEVICE_BGRA_SUPPORT,  // ����� �÷��� ����
            featurelevels, ARRAYSIZE(featurelevels), D3D11_SDK_VERSION,
            &swapchaindesc, &SwapChain, &Device, nullptr, &DeviceContext);

        // ���� ���� Ȯ��
        if (FAILED(hr))
        {
            MessageBox(nullptr, L"Failed to create device and swap chain.", L"Error", MB_OK);
            return;
        }

        // ������ ���� ü���� ���� ��������
        SwapChain->GetDesc(&swapchaindesc);

        // ����Ʈ ���� ����
        ViewportInfo = { 0.0f, 0.0f, (float)swapchaindesc.BufferDesc.Width, (float)swapchaindesc.BufferDesc.Height, 0.0f, 1.0f };
    }


    // Direct3D ��ġ �� ���� ü���� �����ϴ� �Լ�
    void ReleaseDeviceAndSwapChain()
    {
        if (DeviceContext)
        {
            DeviceContext->Flush(); // �����ִ� GPU ��� ����
        }

        if (SwapChain)
        {
            SwapChain->Release();
            SwapChain = nullptr;
        }

        if (Device)
        {
            Device->Release();
            Device = nullptr;
        }

        if (DeviceContext)
        {
            DeviceContext->Release();
            DeviceContext = nullptr;
        }
    }

    // ������ ���۸� �����ϴ� �Լ�
    void CreateFrameBuffer()
    {
        // ���� ü�����κ��� �� ���� �ؽ�ó ��������
        SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&FrameBuffer);

        // ���� Ÿ�� �� ����
        D3D11_RENDER_TARGET_VIEW_DESC framebufferRTVdesc = {};
        framebufferRTVdesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB; // ���� ����
        framebufferRTVdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D; // 2D �ؽ�ó

        Device->CreateRenderTargetView(FrameBuffer, &framebufferRTVdesc, &FrameBufferRTV);
    }

    // ������ ���۸� �����ϴ� �Լ�
    void ReleaseFrameBuffer()
    {
        if (FrameBuffer)
        {
            FrameBuffer->Release();
            FrameBuffer = nullptr;
        }

        if (FrameBufferRTV)
        {
            FrameBufferRTV->Release();
            FrameBufferRTV = nullptr;
        }
    }

    // �����Ͷ����� ���¸� �����ϴ� �Լ�
    void CreateRasterizerState()
    {
        D3D11_RASTERIZER_DESC rasterizerdesc = {};
        rasterizerdesc.FillMode = D3D11_FILL_SOLID; // ä��� ���
        rasterizerdesc.CullMode = D3D11_CULL_BACK; // �� ���̽� �ø�

        Device->CreateRasterizerState(&rasterizerdesc, &RasterizerState);
    }

    // �����Ͷ����� ���¸� �����ϴ� �Լ�
    void ReleaseRasterizerState()
    {
        if (RasterizerState)
        {
            RasterizerState->Release();
            RasterizerState = nullptr;
        }
    }

    // �������� ���� ��� ���ҽ��� �����ϴ� �Լ�
    void Release()
    {
        RasterizerState->Release();

        // ���� Ÿ���� �ʱ�ȭ
        DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

        ReleaseFrameBuffer();
        ReleaseDeviceAndSwapChain();
    }

    // ���� ü���� �� ���ۿ� ����Ʈ ���۸� ��ü�Ͽ� ȭ�鿡 ���
    void SwapBuffer()
    {
        SwapChain->Present(1, 0); // 1: VSync Ȱ��ȭ
    }

    // ���� URenderer Class�� �Ʒ� �ڵ带 �߰� �ϼ���.

    ID3D11VertexShader* SimpleVertexShader;
    ID3D11PixelShader* SimplePixelShader;
    ID3D11InputLayout* SimpleInputLayout;
    unsigned int Stride;

    void CreateShader()
    {
        ID3DBlob* vertexshaderCSO;
        ID3DBlob* pixelshaderCSO;

        D3DCompileFromFile(L"ShaderW0.hlsl", nullptr, nullptr, "mainVS", "vs_5_0", 0, 0, &vertexshaderCSO, nullptr);

        Device->CreateVertexShader(vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), nullptr, &SimpleVertexShader);

        D3DCompileFromFile(L"ShaderW0.hlsl", nullptr, nullptr, "mainPS", "ps_5_0", 0, 0, &pixelshaderCSO, nullptr);

        Device->CreatePixelShader(pixelshaderCSO->GetBufferPointer(), pixelshaderCSO->GetBufferSize(), nullptr, &SimplePixelShader);

        D3D11_INPUT_ELEMENT_DESC layout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        Device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), &SimpleInputLayout);

        Stride = sizeof(FVertexSimple);

        vertexshaderCSO->Release();
        pixelshaderCSO->Release();
    }

    void ReleaseShader()
    {
        if (SimpleInputLayout)
        {
            SimpleInputLayout->Release();
            SimpleInputLayout = nullptr;
        }

        if (SimplePixelShader)
        {
            SimplePixelShader->Release();
            SimplePixelShader = nullptr;
        }

        if (SimpleVertexShader)
        {
            SimpleVertexShader->Release();
            SimpleVertexShader = nullptr;
        }
    }

    // URenderer Class�� �Ʒ� �Լ��� �߰� �ϼ���.
    void Prepare()
    {
        DeviceContext->ClearRenderTargetView(FrameBufferRTV, ClearColor);

        DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        DeviceContext->RSSetViewports(1, &ViewportInfo);
        DeviceContext->RSSetState(RasterizerState);

        DeviceContext->OMSetRenderTargets(1, &FrameBufferRTV, nullptr);
        DeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
    }

    void PrepareShader()
    {
        DeviceContext->VSSetShader(SimpleVertexShader, nullptr, 0);
        DeviceContext->PSSetShader(SimplePixelShader, nullptr, 0);
        DeviceContext->IASetInputLayout(SimpleInputLayout);

        // ���ؽ� ���̴��� ��� ���۸� �����մϴ�.
        if (ConstantBuffer)
        {
            DeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffer);
        }
    }

    void RenderPrimitive(ID3D11Buffer* pBuffer, UINT numVertices)
    {
        UINT offset = 0;
        DeviceContext->IASetVertexBuffers(0, 1, &pBuffer, &Stride, &offset);

        DeviceContext->Draw(numVertices, 0);
    }

    ID3D11Buffer* CreateVertexBuffer(FVertexSimple* vertices, UINT byteWidth)
    {
        // 2. Create a vertex buffer
        D3D11_BUFFER_DESC vertexbufferdesc = {};
        vertexbufferdesc.ByteWidth = byteWidth;
        vertexbufferdesc.Usage = D3D11_USAGE_IMMUTABLE; // will never be updated 
        vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA vertexbufferSRD = { vertices };

        ID3D11Buffer* vertexBuffer;

        Device->CreateBuffer(&vertexbufferdesc, &vertexbufferSRD, &vertexBuffer);

        return vertexBuffer;
    }

    void ReleaseVertexBuffer(ID3D11Buffer* vertexBuffer)
    {
        vertexBuffer->Release();
    }

    struct FConstants
    {
        FVector3 Offset;
        float Scale;
    };


    void CreateConstantBuffer()
    {
        D3D11_BUFFER_DESC constantbufferdesc = {};
        constantbufferdesc.ByteWidth = sizeof(FConstants) + 0xf & 0xfffffff0; // ensure constant buffer size is multiple of 16 bytes
        constantbufferdesc.Usage = D3D11_USAGE_DYNAMIC; // will be updated from CPU every frame
        constantbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        constantbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

        Device->CreateBuffer(&constantbufferdesc, nullptr, &ConstantBuffer);
    }

    void ReleaseConstantBuffer()
    {
        if (ConstantBuffer)
        {
            ConstantBuffer->Release();
            ConstantBuffer = nullptr;
        }
    }

    void UpdateConstant(FVector3 Offset)
    {
        if (ConstantBuffer)
        {
            D3D11_MAPPED_SUBRESOURCE constantbufferMSR;

            DeviceContext->Map(ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantbufferMSR); // update constant buffer every frame
            FConstants* constants = (FConstants*)constantbufferMSR.pData;
            {
                constants->Offset = Offset;
            }
            DeviceContext->Unmap(ConstantBuffer, 0);
        }
    }

    void UpdateConstantWithScale(FVector3 Offset, float Scale)
    {
        if (ConstantBuffer)
        {
            D3D11_MAPPED_SUBRESOURCE constantbufferMSR;
            DeviceContext->Map(ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantbufferMSR);

            // ��� ���ۿ� ��ġ�� ������ ���� ������Ʈ
            FConstants* constants = (FConstants*)constantbufferMSR.pData;
            constants->Offset = Offset;
            constants->Scale = Scale;

            DeviceContext->Unmap(ConstantBuffer, 0);
        }
    }
};


class UBall;
class UBallManager;

bool g_EnableGravity = false;        // �߷� ���� ����
float g_GravityAcceleration = -0.002f; // �߷� ���ӵ� (�Ʒ� ����)

float g_CoefficientOfRestitution = 1.0f;  // ���� �ݹ߰�� (�⺻�� 1.0f == ���� ź�� �浹)

// �ڱ�� ����
bool g_EnableMagneticForce = false;
float g_MagneticStrength = 0.0000001f;

bool g_EnableMouseDrag = false;
bool g_IsDragging = false;
FVector3 g_MouseStartPos;
UBall* g_SelectedBall = nullptr;

enum class EditorMode { Add, Delete };
EditorMode g_EditorMode = EditorMode::Add;
bool g_ShowBallEditor = false;
float g_BallRadius = 0.05f;
bool g_IsNorthPole = true;
bool g_BallPlacementMessage = false;
bool g_BallCannotDeleteMessage = false;
bool g_BallEditorCannotOpen = false;

void HandleBallEditorInput(UBallManager& ballManager, HWND hWnd);

enum class MagneticPole
{
    North,
    South
};

class UBall
{
public:
    FVector3 Location;
    FVector3 Velocity;
    float Radius;
    float Mass;
    MagneticPole Pole;  // ���� �ڱ� �ؼ�

public:
    UBall(FVector3 initialLocation, FVector3 initialVelocity, float radius)
    {
        Location = initialLocation;
        Velocity = initialVelocity;
        Radius = radius;
        Mass = (4.0f / 3.0f) * 3.141592f * Radius * Radius * Radius;
        Pole = (rand() % 2 == 0) ? MagneticPole::North : MagneticPole::South;
    }

    UBall(FVector3 initialLocation, FVector3 initialVelocity, float radius, bool isN)
    {
        Location = initialLocation;
        Velocity = initialVelocity;
        Radius = radius;
        Mass = (4.0f / 3.0f) * 3.141592f * Radius * Radius * Radius;
        Pole = isN ? MagneticPole::North : MagneticPole::South;
    }

    void ApplyGravity()
    {
        if (g_EnableGravity)
        {
            // �߷��� Ȱ��ȭ�� ��� �ӵ��� ���ӵ� �߰�
            Velocity.y += g_GravityAcceleration;
        }
    }

    void ApplyMagneticForce(UBall& other)
    {
        if (!g_EnableMagneticForce) return;

        FVector3 delta = { other.Location.x - Location.x, other.Location.y - Location.y, other.Location.z - Location.z };
        float distanceSquared = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;

        if (distanceSquared < 0.0001f) return;  // �ʹ� ������ ����

        float distance = sqrtf(distanceSquared);
        FVector3 direction = { delta.x / distance, delta.y / distance, delta.z / distance };

        // ���� ���̸� �о��, �ٸ� ���̸� ���
        float forceMultiplier = (Pole == other.Pole) ? -1.0f : 1.0f;
        float forceMagnitude = g_MagneticStrength * forceMultiplier / distanceSquared;

        FVector3 force = { forceMagnitude * direction.x, forceMagnitude * direction.y, forceMagnitude * direction.z };

        // �ӵ��� �ڱ�� ����
        Velocity.x += force.x / Mass;
        Velocity.y += force.y / Mass;
        Velocity.z += force.z / Mass;
    }

    void Move()
    {
        ApplyGravity();
        Location.x += Velocity.x;
        Location.y += Velocity.y;
        Location.z += Velocity.z;
    }

    void HandleCollision(float left, float right, float top, float bottom)
    {
        // X�� �浹 ó��
        if (Location.x - Radius < left)
        {
            Location.x = left + Radius;
            Velocity.x *= -g_CoefficientOfRestitution;    // ���� ź�� �浹
        }
        else if (Location.x + Radius > right)
        {
            Location.x = right - Radius;
            Velocity.x *= -g_CoefficientOfRestitution;
        }

        // Y�� �浹 ó��
        if (Location.y - Radius < top)
        {
            Location.y = top + Radius;
            Velocity.y *= -g_CoefficientOfRestitution;
        }
        else if (Location.y + Radius > bottom)
        {
            Location.y = bottom - Radius;
            Velocity.y *= -g_CoefficientOfRestitution;
        }
    }

    void HandleBallCollision(UBall& other)
    {
        FVector3 delta = { Location.x - other.Location.x, Location.y - other.Location.y, Location.z - other.Location.z };
        float distanceSquared = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;

        // �� ���� �������� ��� �浹 ó��
        float radiusSum = Radius + other.Radius;
        if (distanceSquared < radiusSum * radiusSum)
        {
            float distance = sqrtf(distanceSquared);

            FVector3 normal;
            normal = { delta.x / distance, delta.y / distance, delta.z / distance };

            // ���� ���� ��ġ�� �ʵ��� ��ġ ����
            float penetrationDepth = radiusSum - distance;
            float correctionFactor = 0.5f;  
            FVector3 correction = { normal.x * penetrationDepth * correctionFactor,
                                    normal.y * penetrationDepth * correctionFactor,
                                    normal.z * penetrationDepth * correctionFactor };

            // �� ���� ��ġ�� ����
            Location.x += correction.x;
            Location.y += correction.y;
            Location.z += correction.z;

            other.Location.x -= correction.x;
            other.Location.y -= correction.y;
            other.Location.z -= correction.z;

            FVector3 relativeVelocity = { Velocity.x - other.Velocity.x, Velocity.y - other.Velocity.y, Velocity.z - other.Velocity.z };

            float velocityAlongNormal = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y + relativeVelocity.z * normal.z;

            if (velocityAlongNormal > 0)
                return;

            // ź�� �浹 �ӵ� ��� (�ܼ� �ݻ�� ����)
            float impulseMagnitude = (-(1 + g_CoefficientOfRestitution) * velocityAlongNormal) / (1 / Mass + 1 / other.Mass);

            FVector3 impulse = { impulseMagnitude * normal.x, impulseMagnitude * normal.y, impulseMagnitude * normal.z };

            // �浹 �� �ӵ� ����
            Velocity.x += impulse.x / Mass;
            Velocity.y += impulse.y / Mass;
            Velocity.z += impulse.z / Mass;

            other.Velocity.x -= impulse.x / other.Mass;
            other.Velocity.y -= impulse.y / other.Mass;
            other.Velocity.z -= impulse.z / other.Mass;
        }
    }

    void Render(URenderer& renderer, ID3D11Buffer* vertexBuffer, UINT numVertices)
    {
        renderer.UpdateConstantWithScale(Location, Radius);
        
        renderer.RenderPrimitive(vertexBuffer, numVertices);
    }

    void Update(float left, float right, float top, float bottom)
    {
        Move();
        HandleCollision(left, right, top, bottom);
    }
};

bool showPlacementFailureMessage = false;

class UBallManager
{
public:
    UBall** BallList;
    int CurrentBallCount = 0;
    int Capacity = 0;

    UBallManager()
    {
        BallList = nullptr;

        SYSTEMTIME systemTime;
        GetSystemTime(&systemTime);

        // �õ� �� ���� 
        unsigned int seed = systemTime.wSecond * 1000 + systemTime.wMilliseconds;
        srand(seed);

        AddBall();
    }

    ~UBallManager()
    {
        ClearBalls();
    }


    bool AddBall()
    {

        ResizeCheck();

        const int maxAttempts = 100;
        const float minRadius = 0.05f;
        bool isPlaced = false;

        FVector3 randomVelocity((rand() % 200 - 100) * 0.0001f, (rand() % 200 - 100) * 0.0001f, 0.0f);

        for (int attempt = 0; attempt < maxAttempts; attempt++) 
        {
            FVector3 randomLocation((rand() % 200 - 100) * 0.01f, (rand() % 200 - 100) * 0.01f, 0.0f);
            float randomRadius = (rand() % 15 + 10) * 0.005f;
            

            bool collisionDetected = false;
            for (int i = 0; i < CurrentBallCount; i++) 
            {
                UBall* existingBall = BallList[i];
                float distanceSquared = (existingBall->Location.x - randomLocation.x) * (existingBall->Location.x - randomLocation.x) +
                    (existingBall->Location.y - randomLocation.y) * (existingBall->Location.y - randomLocation.y);

                float radiusSum = randomRadius + existingBall->Radius;
                if (distanceSquared < radiusSum * radiusSum)
                {
                    collisionDetected = true;
                    break;
                }
            }

            if (!collisionDetected)
            {
                BallList[CurrentBallCount] = new UBall(randomLocation, randomVelocity, randomRadius);
                ++CurrentBallCount;
                isPlaced = true;
                break;
            }
        }

        // ���� ���������� ������ ��� �ּ� ���������� �õ�
        if (!isPlaced)
        {
            FVector3 randomLocation((rand() % 200 - 100) * 0.01f, (rand() % 200 - 100) * 0.01f, 0.0f);
            bool collisionDetected = false;

            for (int i = 0; i < CurrentBallCount; ++i)
            {
                UBall* existingBall = BallList[i];
                float distanceSquared = (existingBall->Location.x - randomLocation.x) * (existingBall->Location.x - randomLocation.x) +
                    (existingBall->Location.y - randomLocation.y) * (existingBall->Location.y - randomLocation.y);

                float radiusSum = minRadius + existingBall->Radius;
                if (distanceSquared < radiusSum * radiusSum)
                {
                    collisionDetected = true;
                    break;
                }
            }

            if (!collisionDetected)
            {
                BallList[CurrentBallCount] = new UBall(randomLocation, randomVelocity, minRadius);
                ++CurrentBallCount;
                isPlaced = true;
            }
        }

        // ��� �õ��� ������ ��� ImGui�� ���� �޽��� ���
        if (!isPlaced)
        {
            showPlacementFailureMessage = true;
            return false;
        }

        return true;
    }

    void RemoveRandomBall()
    {
        if (CurrentBallCount <= 1) return;

        int indexToRemove = rand() % CurrentBallCount;
        delete BallList[indexToRemove];

        for (int i = indexToRemove; i < CurrentBallCount - 1; ++i)
        {
            BallList[i] = BallList[i + 1];
        }

        --CurrentBallCount;

        ResizeCheck();
    }

    void Resize(int newCapacity)
    {
        UBall** newBallList = new UBall * [newCapacity];

        for (int i = 0; i < CurrentBallCount; ++i)
        {
            newBallList[i] = BallList[i];
        }

        delete[] BallList;
        BallList = newBallList;
        Capacity = newCapacity;
    }

    void ResizeCheck() {
        if (CurrentBallCount >= Capacity)
        {
            Resize(Capacity == 0 ? 1 : Capacity * 2);
        }
        else {
            if (CurrentBallCount > 0 && CurrentBallCount <= Capacity / 4)
            {
                Resize(Capacity / 2);
            }
        }
    }

    void ClearBalls()
    {
        for (int i = 0; i < CurrentBallCount; ++i)
        {
            delete BallList[i];
        }
        delete[] BallList;
        BallList = nullptr;
        CurrentBallCount = 0;
        Capacity = 0;
    }

    void UpdateBalls(float left, float right, float top, float bottom, HWND hWnd)
    {
        HandleMouseInput(hWnd);  // ���콺 �Է� ó�� �߰�

        // ������ �浹 ó��
        for (int i = 0; i < CurrentBallCount; ++i)
        {
            for (int j = i + 1; j < CurrentBallCount; ++j)
            {
                BallList[i]->HandleBallCollision(*BallList[j]);
            }
        }

        if (g_EnableMagneticForce) {
            for (int i = 0; i < CurrentBallCount; ++i)
            {
                for (int j = 0; j < CurrentBallCount; ++j)
                {
                    if (i != j)
                    {
                        BallList[i]->ApplyMagneticForce(*BallList[j]);
                    }
                }
            }
        }

        // �� ���� ��ġ �� ��� �浹 ó��
        for (int i = 0; i < CurrentBallCount; ++i)
        {
            BallList[i]->Update(left, right, top, bottom);
        }
    }

    void RenderBalls(URenderer& renderer, ID3D11Buffer* vertexBuffer, UINT numVertices)
    {
        for (int i = 0; i < CurrentBallCount; ++i)
        {
            BallList[i]->Render(renderer, vertexBuffer, numVertices);
        }
    }

    void HandleMouseInput(HWND hWnd)
    {
        if (!g_EnableMouseDrag) return;

        // ���콺 ��ġ ��������
        POINT mousePos;
        GetCursorPos(&mousePos);
        ScreenToClient(hWnd, &mousePos);

        // ���� â ũ�⸦ ������
        RECT clientRect;
        GetClientRect(hWnd, &clientRect);
        float windowWidth = static_cast<float>(clientRect.right - clientRect.left);
        float windowHeight = static_cast<float>(clientRect.bottom - clientRect.top);

        // ���콺 ��ġ�� ȭ�� ��ǥ���� -1.0 ~ 1.0 ������ ��ǥ�� ��ȯ
        FVector3 mouseLocation(
            (mousePos.x / windowWidth) * 2.0f - 1.0f,
            -(mousePos.y / windowHeight) * 2.0f + 1.0f,
            0.0f
        );

        static FVector3 lastMouseLocation;
        static FVector3 mouseVelocity;

        // ImGui�� ���� ���콺 Ŭ�� �̺�Ʈ�� �����ϴ� ���¶�� �Լ� ����
        if (ImGui::GetIO().WantCaptureMouse)
        {
            return;
        }

        if (ImGui::IsMouseClicked(0))  // ���콺 Ŭ�� ����
        {
            // Ŭ���� ��ġ�� �ִ� ���� ã��
            for (int i = 0; i < CurrentBallCount; ++i)
            {
                UBall* ball = BallList[i];
                float dx = mouseLocation.x - ball->Location.x;
                float dy = mouseLocation.y - ball->Location.y;
                float distanceSquared = dx * dx + dy * dy;

                if (distanceSquared <= ball->Radius * ball->Radius)  // Ŭ���� ���� �ִ��� Ȯ��
                {
                    g_SelectedBall = ball;
                    g_MouseStartPos = mouseLocation;
                    g_IsDragging = true;
                    lastMouseLocation = mouseLocation;  // ������ ���콺 ��ġ �ʱ�ȭ
                    break;
                }
            }
        }

        if (g_IsDragging)
        {
            if (ImGui::IsMouseDragging(0))  // ���콺�� �巡�� ���� ���
            {
                // ���� ���콺 ��ġ�� �̵���Ŵ
                g_SelectedBall->Location = mouseLocation;

                // ���콺 �ӵ� ��� (���� ��ġ�� ���� ��ġ�� ����)
                mouseVelocity.x = mouseLocation.x - lastMouseLocation.x;
                mouseVelocity.y = mouseLocation.y - lastMouseLocation.y;
                lastMouseLocation = mouseLocation;
            }

            if (ImGui::IsMouseReleased(0))  // ���콺 ��ư�� ������ �巡�� ����
            {
                // ���콺�� ���� �� ������ ���콺 �̵� �ӵ��� ���� �ӵ��� ����
                g_SelectedBall->Velocity = mouseVelocity;

                g_IsDragging = false;
                g_SelectedBall = nullptr;
            }
        }
    }
};

void OffHandleBallEditorErrorMessage() {
    g_BallPlacementMessage = false;
    g_BallCannotDeleteMessage = false;
}

void HandleBallEditorInput(UBallManager& ballManager, HWND hWnd)
{
    if (!g_ShowBallEditor) return;

    ImGui::Begin("Ball Editor", &g_ShowBallEditor);

    // ��� ��ȯ ��ư
    if (g_EditorMode == EditorMode::Add)
    {
        if (ImGui::Button("Add Mode (Click -> Delete Mode)")) 
        {
            g_EditorMode = EditorMode::Delete;
            OffHandleBallEditorErrorMessage();
        }
            

        // �� ���� ���� UI
        ImGui::InputFloat("Radius", &g_BallRadius, 0.01f, 0.1f, "%.2f");
        ImGui::Checkbox("North Pole", &g_IsNorthPole);
    }
    else
    {
        if (ImGui::Button("Delete Mode (Click -> Add Mode)")) 
        {
            g_EditorMode = EditorMode::Add;
            OffHandleBallEditorErrorMessage();
        }
            
    }

    if (ImGui::Button("Close")) {
        g_ShowBallEditor = false;
        OffHandleBallEditorErrorMessage();
    }

    if (g_BallPlacementMessage)
    {
        ImGui::Text("Cannot create a ball because there is already one at the clicked location.");
    }

    if (g_BallCannotDeleteMessage) {
        ImGui::Text("Cannot delete the last remaining ball.");
    }

    ImGui::End();

    // ���콺 �Է� ó��
    if (!ImGui::GetIO().WantCaptureMouse && ImGui::IsMouseClicked(0))
    {
        // ���콺 ��ǥ ��������
        POINT mousePos;
        GetCursorPos(&mousePos);
        ScreenToClient(hWnd, &mousePos);

        // â ũ��� ��� ��ǥ ���
        RECT clientRect;
        GetClientRect(hWnd, &clientRect);
        float windowWidth = static_cast<float>(clientRect.right - clientRect.left);
        float windowHeight = static_cast<float>(clientRect.bottom - clientRect.top);

        FVector3 mouseLocation(
            (mousePos.x / windowWidth) * 2.0f - 1.0f,
            -(mousePos.y / windowHeight) * 2.0f + 1.0f,
            0.0f
        );

        if (g_EditorMode == EditorMode::Add)
        {
            // ���� �ִ��� Ȯ��
            bool collisionDetected = false;
            for (int i = 0; i < ballManager.CurrentBallCount; ++i)
            {
                UBall* ball = ballManager.BallList[i];
                float dx = mouseLocation.x - ball->Location.x;
                float dy = mouseLocation.y - ball->Location.y;
                float distanceSquared = dx * dx + dy * dy;

                if (distanceSquared <= ball->Radius * ball->Radius)
                {
                    collisionDetected = true;
                    break;
                }
            }

            if (!collisionDetected)
            {
                UBall* newBall = new UBall(mouseLocation, FVector3(), g_BallRadius, g_IsNorthPole);
                ballManager.ResizeCheck();
                ballManager.BallList[ballManager.CurrentBallCount++] = newBall;
                g_BallPlacementMessage = false;
            }
            else
            {
                g_BallPlacementMessage = true;
            }
        }
        else if (g_EditorMode == EditorMode::Delete)
        {
            // ���� �ϳ� ���������� ���� ����
            if (ballManager.CurrentBallCount <= 1)
            {
                g_BallCannotDeleteMessage = true;
                return;
            }

            // Ŭ���� �� ����
            for (int i = 0; i < ballManager.CurrentBallCount; ++i)
            {
                UBall* ball = ballManager.BallList[i];
                float dx = mouseLocation.x - ball->Location.x;
                float dy = mouseLocation.y - ball->Location.y;
                float distanceSquared = dx * dx + dy * dy;

                if (distanceSquared <= ball->Radius * ball->Radius)
                {
                    delete ball;
                    for (int j = i; j < ballManager.CurrentBallCount - 1; ++j)
                    {
                        ballManager.BallList[j] = ballManager.BallList[j + 1];
                    }
                    --ballManager.CurrentBallCount;
                    ballManager.ResizeCheck();
                    break;
                }
            }
        }
    }
}







extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// ���� �޽����� ó���� �Լ�
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
    {
        return true;
    }

    switch (message)
    {
    case WM_DESTROY:
        // Signal that the app should quit
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    // ������ Ŭ���� �̸�
    WCHAR WindowClass[] = L"JungleWindowClass";
    WCHAR Title[] = L"Game Tech Lab";

    WNDCLASSW wndclass = { 0, WndProc, 0, 0, 0, 0, 0, 0, 0, WindowClass };
    RegisterClassW(&wndclass);

    HWND hWnd = CreateWindowExW(0, WindowClass, Title, WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1024, 1024, nullptr, nullptr, hInstance, nullptr);

    // Renderer �ʱ�ȭ
    URenderer renderer;
    renderer.Create(hWnd);
    renderer.CreateShader();
    renderer.CreateConstantBuffer();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplWin32_Init((void*)hWnd);
    ImGui_ImplDX11_Init(renderer.Device, renderer.DeviceContext);

    // ���ؽ� ���� ����
    UINT numVerticesSphere = sizeof(sphere_vertices) / sizeof(FVertexSimple);
    ID3D11Buffer* vertexBufferSphere = renderer.CreateVertexBuffer(sphere_vertices, sizeof(sphere_vertices));

    // UBallManager �ʱ�ȭ (1���� ���� �⺻���� ������)
    UBallManager ballManager;

    bool bIsExit = false;

    // ���� �����ϴ� �ڵ带 ���⿡ �߰��մϴ�.

    const float leftBorder = -1.0f;
    const float rightBorder = 1.0f;
    const float topBorder = -1.0f;
    const float bottomBorder = 1.0f;

    // FPS ������ ���� ����
    const int targetFPS = 60;
    const double targetFrameTime = 1000.0 / targetFPS; // �� �������� ��ǥ �ð� (�и��� ����)

    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);

    LARGE_INTEGER startTime, endTime;
    double elapsedTime = 0.0;

    // 저장할 MyClass 객체 생성
    MyClass original;
    original.name = "TestObject_23";
    original.value = 23;

    // 파일에 저장
    SaveMyClassToFile(original, "myclass.json");
    std::cout << "MyClass object saved to myclass.json" << std::endl;

    // 파일에서 MyClass 객체 불러오기
    MyClass loaded = LoadMyClassFromFile("myclass.json");
    
    bool showConsole = true;
    


    while (bIsExit == false)
    {
        QueryPerformanceCounter(&startTime);

        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
            {
                bIsExit = true;
                break;
            }
        }

        renderer.Prepare();
        renderer.PrepareShader();

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ShowExampleAppConsole(&showConsole);

        ImGui::Begin("Jungle Property Window");

        // Ŭ���� �νĵǸ�  PlacementFailureMessage �޽��� â�� ����
        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && ImGui::IsMouseClicked(0))
        {
            showPlacementFailureMessage = false;
        }

        ImGui::Text("Hello Jungle World!");

        // �� ���� ���� UI
        static int desiredBallCount = ballManager.CurrentBallCount;
        if (ImGui::InputInt("Number of Balls", &desiredBallCount))
        {
            if (desiredBallCount < 1) desiredBallCount = 1;

            while (ballManager.CurrentBallCount < desiredBallCount)
            {
                if (!ballManager.AddBall()) {
                    break;
                }
            }

            while (ballManager.CurrentBallCount > desiredBallCount)
            {
                ballManager.RemoveRandomBall();
            }
        }

        desiredBallCount = ballManager.CurrentBallCount;

        // �߷� üũ�ڽ� UI �߰�
        ImGui::Checkbox("Gravity", &g_EnableGravity);

        // �ݹ߰�� �Է� UI
        ImGui::InputFloat("Coefficient of Restitution", &g_CoefficientOfRestitution, 0.01f, 0.1f, "%.2f");

        if (g_CoefficientOfRestitution > 1.0f) g_CoefficientOfRestitution = 1.0f;
        if (g_CoefficientOfRestitution < 0.0f) g_CoefficientOfRestitution = 0.0f;

        ImGui::Checkbox("Enable Magnetic Force", &g_EnableMagneticForce);
        ImGui::InputFloat("Magnetic Strength", &g_MagneticStrength, 0.00000001f, 0.0000001f, "%.8f");

        if (g_MagneticStrength < 0.0f) g_MagneticStrength = 0.0f;

        if (ImGui::Checkbox("Enable Mouse Drag", &g_EnableMouseDrag))
        {
            if (g_EnableMouseDrag && g_ShowBallEditor)
            {
                g_ShowBallEditor = false;  // Mouse Drag Ȱ��ȭ �� Ball Editor ��Ȱ��ȭ
            }
        }

        if (ImGui::Button("Ball Editor"))
        {
            if (g_EnableMouseDrag) {
                g_BallEditorCannotOpen = true;
            }
            else {
                g_BallEditorCannotOpen = false;
                g_ShowBallEditor = true;
            }
        }

        if (g_BallEditorCannotOpen) {
            ImGui::Text("Cannot open Ball Editor while Mouse Drag is enabled.");
        }

        ImGui::End();

        HandleBallEditorInput(ballManager, hWnd);

        ballManager.UpdateBalls(leftBorder, rightBorder, topBorder, bottomBorder, hWnd);
        ballManager.RenderBalls(renderer, vertexBufferSphere, numVerticesSphere);


        if (showPlacementFailureMessage)
        {
            ImGui::Begin("Placement Failure Message", &showPlacementFailureMessage, ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::Text("No space available to place a new ball. Current number of balls: %d", ballManager.CurrentBallCount);
            if (ImGui::Button("Close"))
            {
                showPlacementFailureMessage = false;
            }
            ImGui::End();
        }

        ImGui::Begin("Json Tester");
        ImGui::Text("Loaded MyClass object: \n");
        ImGui::Text("Name: %s\n", loaded.name.c_str());
        ImGui::Text("Value: %d \n", loaded.value);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        renderer.SwapBuffer();

        do
        {
            Sleep(0);

            // ���� ���� �ð� ���
            QueryPerformanceCounter(&endTime);

            // �� �������� �ҿ�� �ð� ��� (�и��� ������ ��ȯ)
            elapsedTime = (endTime.QuadPart - startTime.QuadPart) * 1000.0 / frequency.QuadPart;

        } while (elapsedTime < targetFrameTime);

    }

    // �Ҹ�
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    renderer.ReleaseVertexBuffer(vertexBufferSphere);
    renderer.ReleaseConstantBuffer();
    renderer.ReleaseShader();
    renderer.Release();

    return 0;
}
