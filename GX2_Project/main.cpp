// D3D LAB 1a "Line Land".
// Author: L.Norri CD DRX, FullSail University

// Introduction:
// Welcome to the first lab of the Direct3D Graphics Programming class.
// This is the ONLY guided lab in this course! 
// Future labs will demand the habits & foundation you develop right now!  
// It is CRITICAL that you follow each and every step. ESPECIALLY THE READING!!!

//************************************************************
//************ INCLUDES & DEFINES ****************************
//************************************************************

#include <iostream>
#include <ctime>
#include "XTime.h"
#include <vector>
#include <vld.h>
using namespace std;

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#include <thread>
#include <atlbase.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
using namespace DirectX;

#include "DDSTextureLoader.h"
#include "safe_thread.h"
#include <atomic>

#include "Trivial_PS.csh"
#include "Trivial_VS.csh"
#include "SkyboxVertexShader.csh"
#include "SkyboxPixelShader.csh"
#include "MiniGrid_PS.csh"
#include "MiniGrid_GS.csh"
#include "BillboardGS.csh"
#include "GeomtryVS.csh"
#include "InstancedExampleGS.csh"
#include "SecondViewportPS.csh"
#include "InstanceVShader.csh"

#define BACKBUFFER_WIDTH	1024
#define BACKBUFFER_HEIGHT	768

//************************************************************
//************ SIMPLE WINDOWS APP CLASS **********************
//************************************************************
struct SIMPLE_VERTEX
{
	XMFLOAT3 pos;
	XMFLOAT3 UVs;
	XMFLOAT3 normals;
};

class DEMO_APP
{
	HINSTANCE						application;
	WNDPROC							appWndProc;
	HWND							window;

	CComPtr<IDXGISwapChain								>pSwapChain;
	CComPtr<ID3D11Device								>pDevice;
	CComPtr<ID3D11DeviceContext							>pDeviceContext;
	CComPtr<ID3D11RenderTargetView						>RTV;
	CComPtr<ID3D11RenderTargetView                      >SecondViewRTV;
	//floor variables
	CComPtr<ID3D11SamplerState							>pFloorSamplerState;
	CComPtr<ID3D11ShaderResourceView					>pFloorSRV;
	CComPtr<ID3D11Buffer								>FloorVertexBuffer;
	CComPtr<ID3D11Buffer								>FloorIndexBuffer;
	//skybox stuff
	CComPtr<ID3D11Buffer								>pSkyboxVBuffer;
	CComPtr<ID3D11Buffer								>pSkyboxIBuffer;
	CComPtr<ID3D11ShaderResourceView					>pSkyboxSRV;
	CComPtr<ID3D11SamplerState							>SkyboxSampler;
	//ARCFIGHTER
	CComPtr<ID3D11Buffer								>pARCFighterVBuffer;
	CComPtr<ID3D11ShaderResourceView					>pARCFighterSRV;
	CComPtr<ID3D11SamplerState							>pARCFighterSampler;
	unsigned int ARCVertCount;
	//GOOMBA
	CComPtr<ID3D11Buffer								>pGoombaVBuff;
	CComPtr<ID3D11SamplerState							>pGoombaSampler;
	unsigned int GoombaVertCount;

	CComPtr<ID3D11ShaderResourceView					>MultiTextureProof;
	CComPtr<ID3D11SamplerState							>MultitextureSampler;

	CComPtr<ID3D11Resource								>pBB;
	CComPtr<ID3D11Texture2D								>ZBuffer;
	CComPtr<ID3D11DepthStencilView						>DPV;

	CComPtr<ID3D11InputLayout							>InputLayout;
	CComPtr<ID3D11VertexShader							>pObjectVS;
	CComPtr<ID3D11PixelShader							>pLightingPS;
	CComPtr<ID3D11VertexShader							>SkyboxVS;
	CComPtr<ID3D11PixelShader							>SkyboxPS;
	CComPtr<ID3D11PixelShader							>GSPixelShader;
	CComPtr<ID3D11GeometryShader						>TriangleGS;
	CComPtr<ID3D11VertexShader							>TriangleVS;
	CComPtr<ID3D11PixelShader							>SecondViewPS;
	CComPtr<ID3D11GeometryShader						>SecondViewGS;
	CComPtr<ID3D11GeometryShader						>InstancedGS;
	CComPtr<ID3D11VertexShader							>VS_Instanced;

	CComPtr<ID3D11Buffer								>SkyboxConstBuffer;
	CComPtr<ID3D11Buffer								>FloorConstBuffer;
	CComPtr<ID3D11Buffer								>ARCConstBuffer;
	CComPtr<ID3D11Buffer								>GoombaConstBuff;
	CComPtr<ID3D11Buffer								>ConstantBufferToSendScene;
	CComPtr<ID3D11Buffer								>PointLightConstBuff;
	CComPtr<ID3D11Buffer								>SpotLightConstBuff;
	CComPtr<ID3D11Buffer								>DirectLightConstBuff;
	CComPtr<ID3D11Buffer								>InstancedVBuff;
	CComPtr<ID3D11Buffer								>MultiTextureConstBuff;
	CComPtr<ID3D11Buffer								>MirageConstBuff;

	//frame for GS
	CComPtr<ID3D11Buffer								>GSVertexBuffer;
	CComPtr<ID3D11Buffer								>GSConstBuffer;
	CComPtr<ID3D11ShaderResourceView					>FrameSRV;
	CComPtr<ID3D11SamplerState							>FrameSamplerState;

	CComPtr<ID3D11ShaderResourceView					>InstancedSRV;
	CComPtr<ID3D11SamplerState							>InstanceSampler;

	CComPtr<ID3D11DeviceContext							>deferredCont;

	//offscreen items
	D3D11_VIEWPORT										*offScreenVP;
	CComPtr<ID3D11RenderTargetView						>GoombaRTV;
	CComPtr<ID3D11ShaderResourceView					>GoombaSRV;
	CComPtr<ID3D11ShaderResourceView					>OffscreenSRV;
	CComPtr<ID3D11ShaderResourceView					>SecondViewSRV;
	CComPtr<ID3D11Texture2D								>DrawTexture;
	CComPtr<ID3D11Texture2D								>OffscreenT2D;
	CComPtr<ID3D11Texture2D								>SecondViewT2D;
	CComPtr<ID3D11Texture2D								>SecondViewDepth;
	CComPtr<ID3D11DepthStencilView						>GoombaDPV;
	CComPtr<ID3D11DepthStencilView						>SecondViewDPV;
	CComPtr<ID3D11Buffer								>ViewportPlainVBuff;
	CComPtr<ID3D11Buffer								>ViewportPlainIBuff;
	CComPtr<ID3D11Buffer								>ViewportConstBuff;
	
	D3D11_VIEWPORT *viewport;
	D3D11_VIEWPORT *OffScreenViewPort;
	D3D11_VIEWPORT *SecondViewport;
	D3D_FEATURE_LEVEL *pFeatureLevel;

	CComPtr<ID3D11Debug> pDebug; 
	struct SEND_WORLD_MATRIX{
		XMFLOAT4X4 worldMatrix;
	};
	struct SEND_SCENE_MATRICES{
		XMFLOAT4X4 viewMatrix;
		XMFLOAT4X4 projectionMatrix;
	};
	struct POINT_LIGHT{
		XMFLOAT4 color;
		XMFLOAT3 position;
		float radius;
	};
	struct SPOT_LIGHT{
		XMFLOAT4 color;
		XMFLOAT3 position;
		float radius;
		XMFLOAT3 ConeDir;
		float innerRatio;
		float outerRatio;
		XMFLOAT3 padding;
	};
	struct DIRECTIONAL_LIGHT{
		XMFLOAT4 color;
		XMFLOAT4 direction;
	};
	struct MULTITEXTURE_BOOL{
		XMFLOAT4 multiple;
	};
	struct TIME_HERE{
		XMFLOAT4 time;
	};
	SEND_WORLD_MATRIX worldMatrix;
	SEND_SCENE_MATRICES sceneMatrices;
	SEND_SCENE_MATRICES OffscreenMatrices;
	SEND_SCENE_MATRICES SecondViewMatrices;
	SEND_WORLD_MATRIX SkyboxWorld;
	SEND_WORLD_MATRIX SkyboxWorldForSecondViewport;
	SEND_WORLD_MATRIX FloorWorld;
	SEND_WORLD_MATRIX BackDrop;
	SEND_WORLD_MATRIX ARC170World[3];
	SEND_WORLD_MATRIX GoombaWorld;
	SEND_WORLD_MATRIX GoombaCubeWorld;
	SEND_WORLD_MATRIX TriangleWorld;
	SEND_WORLD_MATRIX ViewPlainWorld;
	SEND_WORLD_MATRIX InstancedWorld;
	MULTITEXTURE_BOOL WeHaveMultiTextures;
	MULTITEXTURE_BOOL NoMultiTextures;
	XMFLOAT4 GoombaPostion;
	XMFLOAT4 PLPosition;
	POINT_LIGHT pointLight;
	SPOT_LIGHT spotLight;
	DIRECTIONAL_LIGHT directLight;
	TIME_HERE TheTime;

	XTime *ttime;

	thread thred;
	atomic<ID3D11CommandList*> pCommandList;
	atomic_bool RunningThread;
	XMFLOAT2 velocity;
	XMFLOAT2 PLVelocity;
	XMFLOAT2 SLVelocity;

	float rotation = 0;
public:
	

	DEMO_APP(HINSTANCE hinst, WNDPROC proc);
	bool Run();
	bool ShutDown();

	friend void BIGLOAD(DEMO_APP *myApple, wstring wideString);
	bool LoadOBJ(const char *path, vector<XMFLOAT3> &out_Verts, vector<XMFLOAT3> &out_UVs, vector<XMFLOAT3> &out_Norms);
	void MoveSkybox();
	void MoveCamera();
	void MoveGoomba(double timer);
	void MoveLights(double timer);
	void DrawSecondView();
};
void BIGLOAD(DEMO_APP *myApple, wstring wideString)
{
	HRESULT hr;
	hr = CreateDDSTextureFromFile(myApple->pDevice, wideString.c_str(), &myApple->pBB.p, &myApple->pFloorSRV.p);

	while (myApple->RunningThread)
	{
		if (myApple->pCommandList.load() == nullptr)
		{
			D3D11_MAPPED_SUBRESOURCE MapSubresource;
			unsigned int VertexSize = sizeof(SIMPLE_VERTEX);
			unsigned int Array[] = { 0 };
			float Colour[4] = { 200.0f / 255.0f, 0, 220.0f / 225.0f, 0 / 255.0f };

			myApple->deferredCont->RSSetViewports(1, myApple->SecondViewport);
			myApple->deferredCont->OMSetRenderTargets(1, &myApple->SecondViewRTV.p, myApple->SecondViewDPV);

			memset(&MapSubresource, 0, sizeof(MapSubresource));
			myApple->deferredCont->Map(myApple->FloorConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
			memcpy(MapSubresource.pData, &myApple->FloorWorld, sizeof(myApple->FloorWorld));
			myApple->deferredCont->Unmap(myApple->FloorConstBuffer, 0);

			memset(&MapSubresource, 0, sizeof(MapSubresource));
			myApple->deferredCont->Map(myApple->ConstantBufferToSendScene, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
			memcpy(MapSubresource.pData, &myApple->SecondViewMatrices, sizeof(myApple->SecondViewMatrices));
			myApple->deferredCont->Unmap(myApple->ConstantBufferToSendScene, 0);

			memset(&MapSubresource, 0, sizeof(MapSubresource));
			myApple->deferredCont->Map(myApple->DirectLightConstBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
			memcpy(MapSubresource.pData, &myApple->directLight, sizeof(myApple->directLight));
			myApple->deferredCont->Unmap(myApple->DirectLightConstBuff, 0);

			memset(&MapSubresource, 0, sizeof(MapSubresource));
			myApple->deferredCont->Map(myApple->PointLightConstBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
			memcpy(MapSubresource.pData, &myApple->pointLight, sizeof(myApple->pointLight));
			myApple->deferredCont->Unmap(myApple->PointLightConstBuff, 0);

			memset(&MapSubresource, 0, sizeof(MapSubresource));
			myApple->deferredCont->Map(myApple->SpotLightConstBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
			memcpy(MapSubresource.pData, &myApple->spotLight, sizeof(myApple->spotLight));
			myApple->deferredCont->Unmap(myApple->SpotLightConstBuff, 0);

			memset(&MapSubresource, 0, sizeof(MapSubresource));
			myApple->deferredCont->Map(myApple->MultiTextureConstBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
			memcpy(MapSubresource.pData, &myApple->WeHaveMultiTextures, sizeof(myApple->WeHaveMultiTextures));
			myApple->deferredCont->Unmap(myApple->MultiTextureConstBuff, 0);

			myApple->deferredCont->PSSetConstantBuffers(0, 1, &myApple->PointLightConstBuff.p);
			myApple->deferredCont->PSSetConstantBuffers(1, 1, &myApple->SpotLightConstBuff.p);
			myApple->deferredCont->PSSetConstantBuffers(2, 1, &myApple->DirectLightConstBuff.p);
			myApple->deferredCont->PSSetConstantBuffers(3, 1, &myApple->MultiTextureConstBuff.p);
			myApple->deferredCont->VSSetConstantBuffers(0, 1, &myApple->FloorConstBuffer.p);
			myApple->deferredCont->VSSetConstantBuffers(1, 1, &myApple->ConstantBufferToSendScene.p);
			myApple->deferredCont->PSSetShaderResources(0, 1, &myApple->pFloorSRV.p);
			myApple->deferredCont->PSSetShaderResources(1, 1, &myApple->MultiTextureProof.p);
			myApple->deferredCont->PSSetSamplers(0, 1, &myApple->pFloorSamplerState.p);
			myApple->deferredCont->IASetVertexBuffers(0, 1, &myApple->FloorVertexBuffer.p, &VertexSize, Array);
			myApple->deferredCont->IASetIndexBuffer(myApple->FloorIndexBuffer.p, DXGI_FORMAT_R32_UINT, 0);
			myApple->deferredCont->PSSetShader(myApple->pLightingPS.p, NULL, 0);
			myApple->deferredCont->VSSetShader(myApple->pObjectVS.p, NULL, 0);
			myApple->deferredCont->IASetInputLayout(myApple->InputLayout.p);
			myApple->deferredCont->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			myApple->deferredCont->DrawIndexed(6, 0, 0);


			myApple->deferredCont->ClearState();

			myApple->deferredCont->RSSetViewports(1, myApple->viewport);
			myApple->deferredCont->OMSetRenderTargets(1, &myApple->RTV.p, myApple->DPV);
		
			memset(&MapSubresource, 0, sizeof(MapSubresource));
			myApple->deferredCont->Map(myApple->FloorConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
			memcpy(MapSubresource.pData, &myApple->FloorWorld, sizeof(myApple->FloorWorld));
			myApple->deferredCont->Unmap(myApple->FloorConstBuffer, 0);

			memset(&MapSubresource, 0, sizeof(MapSubresource));
			myApple->deferredCont->Map(myApple->ConstantBufferToSendScene, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
			memcpy(MapSubresource.pData, &myApple->sceneMatrices, sizeof(myApple->sceneMatrices));
			myApple->deferredCont->Unmap(myApple->ConstantBufferToSendScene, 0);

			memset(&MapSubresource, 0, sizeof(MapSubresource));
			myApple->deferredCont->Map(myApple->DirectLightConstBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
			memcpy(MapSubresource.pData, &myApple->directLight, sizeof(myApple->directLight));
			myApple->deferredCont->Unmap(myApple->DirectLightConstBuff, 0);

			memset(&MapSubresource, 0, sizeof(MapSubresource));
			myApple->deferredCont->Map(myApple->PointLightConstBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
			memcpy(MapSubresource.pData, &myApple->pointLight, sizeof(myApple->pointLight));
			myApple->deferredCont->Unmap(myApple->PointLightConstBuff, 0);

			memset(&MapSubresource, 0, sizeof(MapSubresource));
			myApple->deferredCont->Map(myApple->SpotLightConstBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
			memcpy(MapSubresource.pData, &myApple->spotLight, sizeof(myApple->spotLight));
			myApple->deferredCont->Unmap(myApple->SpotLightConstBuff, 0);

			memset(&MapSubresource, 0, sizeof(MapSubresource));
			myApple->deferredCont->Map(myApple->MultiTextureConstBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
			memcpy(MapSubresource.pData, &myApple->WeHaveMultiTextures, sizeof(myApple->WeHaveMultiTextures));
			myApple->deferredCont->Unmap(myApple->MultiTextureConstBuff, 0);

			myApple->deferredCont->PSSetConstantBuffers(0, 1, &myApple->PointLightConstBuff.p);
			myApple->deferredCont->PSSetConstantBuffers(1, 1, &myApple->SpotLightConstBuff.p);
			myApple->deferredCont->PSSetConstantBuffers(2, 1, &myApple->DirectLightConstBuff.p);
			myApple->deferredCont->PSSetConstantBuffers(3, 1, &myApple->MultiTextureConstBuff.p);
			myApple->deferredCont->VSSetConstantBuffers(0, 1, &myApple->FloorConstBuffer.p);
			myApple->deferredCont->VSSetConstantBuffers(1, 1, &myApple->ConstantBufferToSendScene.p);
			myApple->deferredCont->PSSetShaderResources(0, 1, &myApple->pFloorSRV.p);
			myApple->deferredCont->PSSetShaderResources(1, 1, &myApple->MultiTextureProof.p);
			myApple->deferredCont->PSSetSamplers(0, 1, &myApple->pFloorSamplerState.p);
			myApple->deferredCont->IASetVertexBuffers(0, 1, &myApple->FloorVertexBuffer.p, &VertexSize, Array);
			myApple->deferredCont->IASetIndexBuffer(myApple->FloorIndexBuffer.p, DXGI_FORMAT_R32_UINT, 0);
			myApple->deferredCont->PSSetShader(myApple->pLightingPS.p, NULL, 0);
			myApple->deferredCont->VSSetShader(myApple->pObjectVS.p, NULL, 0);
			myApple->deferredCont->IASetInputLayout(myApple->InputLayout.p);
			myApple->deferredCont->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			myApple->deferredCont->DrawIndexed(6, 0, 0);

			ID3D11CommandList *commandList_temp = myApple->pCommandList.load();
			myApple->deferredCont->FinishCommandList(true, &commandList_temp);
			myApple->pCommandList.store(commandList_temp);


		}
		else
		{
			Sleep(0);
		}
	}
}
bool DEMO_APP::LoadOBJ(const char *path, vector<XMFLOAT3> &out_Verts, vector<XMFLOAT3> &out_UVs, vector<XMFLOAT3> &out_Norms)
{
	vector<unsigned int> vertIndices, uvIndices, normIndices;
	vector<XMFLOAT3> temp_Verts;
	vector<XMFLOAT3> temp_UVs;
	vector<XMFLOAT3> temp_Norms;

	FILE *file = fopen(path, "r");
	if (file == NULL)
	{
		printf("Cannot open file!\n");
		return false;
	}

	while (1){
		char lineHeader[128];
		//first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
		{
			int x = 64764764;
			break; //EOF - end of file
		}

		if (strcmp(lineHeader, "v") == 0)
		{
			XMFLOAT3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_Verts.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			XMFLOAT3 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = 1.0f - uv.y;
			temp_UVs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			XMFLOAT3 normals;
			fscanf(file, "%f %f %f\n", &normals.x, &normals.y, &normals.z);

			temp_Norms.push_back(normals);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			unsigned int vertexIndex[4] = { -1 }, uvIndex[4] = { -1 }, normalIndex[4] = { -1 };
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], 
				&normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], 
				&vertexIndex[2], &uvIndex[2], &normalIndex[2], &vertexIndex[3], &uvIndex[3], &normalIndex[3]);

			if (matches != 12)
			{
				if (matches != 9)
				{
					printf("File can't be read by parser"); 
					return false;
				}
				vertIndices.push_back(vertexIndex[0] - 1);
				vertIndices.push_back(vertexIndex[1] - 1);
				vertIndices.push_back(vertexIndex[2] - 1);
				uvIndices.push_back(uvIndex[0] - 1);
				uvIndices.push_back(uvIndex[1] - 1);
				uvIndices.push_back(uvIndex[2] - 1);
				normIndices.push_back(normalIndex[0] - 1);
				normIndices.push_back(normalIndex[1] - 1);
				normIndices.push_back(normalIndex[2] - 1);
			}
			else
			{
				//first triangle
				vertIndices.push_back(vertexIndex[0] - 1);
				vertIndices.push_back(vertexIndex[1] - 1);
				vertIndices.push_back(vertexIndex[3] - 1);
				uvIndices.push_back(uvIndex[0] - 1);
				uvIndices.push_back(uvIndex[1] - 1);
				uvIndices.push_back(uvIndex[3] - 1);
				normIndices.push_back(normalIndex[0] - 1);
				normIndices.push_back(normalIndex[1] - 1);
				normIndices.push_back(normalIndex[3] - 1);
				//second triangle
				vertIndices.push_back(vertexIndex[3] - 1);
				vertIndices.push_back(vertexIndex[1] - 1);
				vertIndices.push_back(vertexIndex[2] - 1);
				uvIndices.push_back(uvIndex[3] - 1);
				uvIndices.push_back(uvIndex[1] - 1);
				uvIndices.push_back(uvIndex[2] - 1);
				normIndices.push_back(normalIndex[3] - 1);
				normIndices.push_back(normalIndex[1] - 1);
				normIndices.push_back(normalIndex[2] - 1);
			}
		}
	}

	for (unsigned int i = 0; i < vertIndices.size(); i++)
	{
		unsigned int VertexIndex = vertIndices[i];
		XMFLOAT3 aVert = temp_Verts[VertexIndex];
		out_Verts.push_back(aVert);
	}
	for (unsigned int i = 0; i < uvIndices.size(); i++)
	{
		unsigned int UVIndex = uvIndices[i];
		XMFLOAT3 aUV = temp_UVs[UVIndex];
		out_UVs.push_back(aUV);
	}
	for (unsigned int i = 0; i < normIndices.size(); i++)
	{
		unsigned int NormalIndex = normIndices[i];
		XMFLOAT3 aNorm = temp_Norms[NormalIndex];
		out_Norms.push_back(aNorm);
	}
	fclose(file);
	return true;
}
//************************************************************
//************ CREATION OF OBJECTS & RESOURCES ***************
//************************************************************

DEMO_APP::DEMO_APP(HINSTANCE hinst, WNDPROC proc)
{
	// ****************** BEGIN WARNING ***********************// 
	// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY! 
	application = hinst;
	appWndProc = proc;

	WNDCLASSEX  wndClass;
	ZeroMemory(&wndClass, sizeof(wndClass));
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.lpfnWndProc = appWndProc;
	wndClass.lpszClassName = L"DirectXApplication";
	wndClass.hInstance = application;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME);
	//wndClass.hIcon			= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_FSICON));
	RegisterClassEx(&wndClass);

	RECT window_size = { 0, 0, BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT };
	AdjustWindowRect(&window_size, WS_OVERLAPPEDWINDOW, false);

	window = CreateWindow(L"DirectXApplication", L"GX2 Project", WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX),
		CW_USEDEFAULT, CW_USEDEFAULT, window_size.right - window_size.left, window_size.bottom - window_size.top,
		NULL, NULL, application, this);

	ShowWindow(window, SW_SHOW);
	//********************* END WARNING ************************//
	// TODO: PART 1 STEP 3a
	DXGI_SWAP_CHAIN_DESC swapchain;
	memset(&swapchain, 0, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapchain.BufferCount = 1;
	swapchain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchain.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapchain.OutputWindow = window;
	swapchain.Windowed = true;
	swapchain.SampleDesc.Count = 1;
	swapchain.SampleDesc.Quality = 0;

#if _DEBUG
	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D10_CREATE_DEVICE_DEBUG,
		nullptr, 0, D3D11_SDK_VERSION, &swapchain, &pSwapChain,
		&pDevice, NULL, &pDeviceContext);
#else
	D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
		nullptr, 0, D3D11_SDK_VERSION, &swapchain, &pSwapChain,
		&pDevice, NULL, &pDeviceContext);
#endif // _DEBUG

	pSwapChain->GetBuffer(0, __uuidof(pBB), reinterpret_cast<void**>(&pBB));
	pDevice->CreateRenderTargetView(pBB, NULL, &RTV);

	viewport = new D3D11_VIEWPORT;
	SecondViewport = new D3D11_VIEWPORT;

	DXGI_SWAP_CHAIN_DESC temp;
	pSwapChain->GetDesc(&temp);
	viewport->Height = (float)temp.BufferDesc.Height;
	viewport->Width = (float)temp.BufferDesc.Width;
	viewport->MinDepth = 0;
	viewport->MaxDepth = 1;
	viewport->TopLeftX = 0;
	viewport->TopLeftY = 0;

	//second viewport
	SecondViewport->TopLeftX = 0;
	SecondViewport->TopLeftY = 0;
	SecondViewport->Height = 300.0f;
	SecondViewport->Width = 300.0f;
	SecondViewport->MinDepth = 0;
	SecondViewport->MaxDepth = 1.0f;

	//second view of scene
	D3D11_TEXTURE2D_DESC SecondViewDesc2D;
	memset(&SecondViewDesc2D, 0, sizeof(D3D11_TEXTURE2D_DESC));
	SecondViewDesc2D.Width = 300;
	SecondViewDesc2D.Height = 300;
	SecondViewDesc2D.MipLevels = 1;
	SecondViewDesc2D.ArraySize = 1;
	SecondViewDesc2D.SampleDesc.Count = 1;
	SecondViewDesc2D.SampleDesc.Quality = 0;
	SecondViewDesc2D.Usage = D3D11_USAGE_DEFAULT;
	SecondViewDesc2D.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	SecondViewDesc2D.Format = DXGI_FORMAT_D32_FLOAT;
	//offscreen texture2d
	pDevice->CreateTexture2D(&SecondViewDesc2D, NULL, &SecondViewDepth);

	memset(&SecondViewDesc2D, 0, sizeof(D3D11_TEXTURE2D_DESC));
	SecondViewDesc2D.Width = 300;
	SecondViewDesc2D.Height = 300;
	SecondViewDesc2D.MipLevels = 1;
	SecondViewDesc2D.ArraySize = 1;
	SecondViewDesc2D.SampleDesc.Count = 1;
	SecondViewDesc2D.SampleDesc.Quality = 0;
	SecondViewDesc2D.Usage = D3D11_USAGE_DEFAULT;
	SecondViewDesc2D.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	SecondViewDesc2D.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	SecondViewDesc2D.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	pDevice->CreateTexture2D(&SecondViewDesc2D, NULL, &SecondViewT2D);

	D3D11_DEPTH_STENCIL_VIEW_DESC SecondViewDSVDesc;
	memset(&SecondViewDSVDesc, 0, sizeof(SecondViewDSVDesc));
	SecondViewDSVDesc.Format = DXGI_FORMAT_D32_FLOAT;
	SecondViewDSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	//new Depth Stencil
	pDevice->CreateDepthStencilView(SecondViewDepth, &SecondViewDSVDesc, &SecondViewDPV);
	//New render target view
	pDevice->CreateRenderTargetView(SecondViewT2D, NULL, &SecondViewRTV);

	//creating new shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	memset(&srvDesc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Texture2D.MipLevels = -1;
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	pDevice->CreateShaderResourceView(SecondViewT2D, &srvDesc, &SecondViewSRV);

	//set up offscreen depthstencil
	D3D11_TEXTURE2D_DESC goombaT2Ddesc;
	memset(&goombaT2Ddesc, 0, sizeof(D3D11_TEXTURE2D_DESC));
	goombaT2Ddesc.Width = 400;
	goombaT2Ddesc.Height = 400;
	goombaT2Ddesc.MipLevels = 1;
	goombaT2Ddesc.ArraySize = 1;
	goombaT2Ddesc.SampleDesc.Count = 1;
	goombaT2Ddesc.SampleDesc.Quality = 0;
	goombaT2Ddesc.Usage = D3D11_USAGE_DEFAULT;
	goombaT2Ddesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	goombaT2Ddesc.Format = DXGI_FORMAT_D32_FLOAT;
	//offscreen texture2d
	pDevice->CreateTexture2D(&goombaT2Ddesc, NULL, &OffscreenT2D);

	D3D11_TEXTURE2D_DESC DrawTextureDesc;
	memset(&DrawTextureDesc, 0, sizeof(D3D11_TEXTURE2D_DESC));
	DrawTextureDesc.Width = 400;
	DrawTextureDesc.Height = 400;
	DrawTextureDesc.MipLevels = 1;
	DrawTextureDesc.ArraySize = 1;
	DrawTextureDesc.SampleDesc.Count = 1;
	DrawTextureDesc.SampleDesc.Quality = 0;
	DrawTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	DrawTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	DrawTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	DrawTextureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	pDevice->CreateTexture2D(&DrawTextureDesc, NULL, &DrawTexture);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	memset(&dsvDesc, 0, sizeof(dsvDesc));
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	//new Depth Stencil
	pDevice->CreateDepthStencilView(OffscreenT2D, &dsvDesc, &GoombaDPV);
	//New render target view
	pDevice->CreateRenderTargetView(DrawTexture, NULL, &GoombaRTV);

	//creating new shader resource view
	memset(&srvDesc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Texture2D.MipLevels = -1;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	pDevice->CreateShaderResourceView(DrawTexture, &srvDesc, &OffscreenSRV);

	//second viewport for goomba
	OffScreenViewPort = new D3D11_VIEWPORT;
	pSwapChain->GetDesc(&temp);
	OffScreenViewPort->Height = 400.0f;
	OffScreenViewPort->Width = 400.0f;
	OffScreenViewPort->MinDepth = 0;
	OffScreenViewPort->MaxDepth = 1;
	OffScreenViewPort->TopLeftX = 0;
	OffScreenViewPort->TopLeftY = 0;

	//create DeferredContext
	pDevice->CreateDeferredContext(0, &deferredCont);

	//samplers
	D3D11_SAMPLER_DESC samplerDesc;
	memset(&samplerDesc, 0, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	pDevice->CreateSamplerState(&samplerDesc, &pFloorSamplerState);
	pDevice->CreateSamplerState(&samplerDesc, &pARCFighterSampler);
	pDevice->CreateSamplerState(&samplerDesc, &pGoombaSampler);
	pDevice->CreateSamplerState(&samplerDesc, &FrameSamplerState);
	pDevice->CreateSamplerState(&samplerDesc, &InstanceSampler);
	pDevice->CreateSamplerState(&samplerDesc, &MultitextureSampler);

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;

	pDevice->CreateSamplerState(&samplerDesc, &SkyboxSampler);

	//skybox creation
	SIMPLE_VERTEX SkyboxVerts[8];
	SkyboxVerts[0].pos = { -1.0f, -1.0f, -1.0f };
	SkyboxVerts[1].pos = { -1.0f, 1.0f, -1.0f };
	SkyboxVerts[2].pos = { 1.0f, 1.0f, -1.0f };
	SkyboxVerts[3].pos = { 1.0f, -1.0f, -1.0f };
	SkyboxVerts[4].pos = { -1.0f, -1.0f, 1.0f };
	SkyboxVerts[5].pos = { -1.0f, 1.0f, 1.0f };
	SkyboxVerts[6].pos = { 1.0f, 1.0f, 1.0f };
	SkyboxVerts[7].pos = { 1.0f, -1.0f, 1.0f };

	D3D11_BUFFER_DESC SkyboxDesc;
	ZeroMemory(&SkyboxDesc, sizeof(D3D11_BUFFER_DESC));
	SkyboxDesc.Usage = D3D11_USAGE_IMMUTABLE;
	SkyboxDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	SkyboxDesc.CPUAccessFlags = NULL;
	SkyboxDesc.ByteWidth = sizeof(SIMPLE_VERTEX) * 8;
	D3D11_SUBRESOURCE_DATA SBSubresource;
	ZeroMemory(&SBSubresource, sizeof(D3D11_SUBRESOURCE_DATA));
	SBSubresource.pSysMem = SkyboxVerts;
	pDevice->CreateBuffer(&SkyboxDesc, &SBSubresource, &pSkyboxVBuffer);

	D3D11_BUFFER_DESC SBIndexDesc;
	ZeroMemory(&SBIndexDesc, sizeof(D3D11_BUFFER_DESC));
	SBIndexDesc.Usage = D3D11_USAGE_IMMUTABLE;
	SBIndexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	SBIndexDesc.CPUAccessFlags = NULL;
	SBIndexDesc.ByteWidth = sizeof(unsigned int) * 36;

	D3D11_SUBRESOURCE_DATA SBIndexResource;
	ZeroMemory(&SBIndexResource, sizeof(D3D11_SUBRESOURCE_DATA));
	unsigned int SBindices[36] =
	{
		0, 2, 1,
		0, 3, 2,
		4, 1, 5,
		4, 0, 1,
		4, 7, 3,
		4, 3, 0,
		3, 6, 2,
		3, 7, 6,
		7, 5, 6,
		7, 4, 5,
		1, 6, 5,
		1, 2, 6
	};
	SBIndexResource.pSysMem = SBindices;
	pDevice->CreateBuffer(&SBIndexDesc, &SBIndexResource, &pSkyboxIBuffer);

	//floor plain
	SIMPLE_VERTEX floorVerts[4];
	floorVerts[0].pos = { -5, 0, 5 };
	floorVerts[1].pos = { 5, 0, 5 };
	floorVerts[2].pos = { -5, 0, -5 };
	floorVerts[3].pos = { 5, 0, -5 };
	floorVerts[0].UVs = { 0, 0, 0 };
	floorVerts[1].UVs = { 1, 0, 0 };
	floorVerts[2].UVs = { 0, 1, 0 };
	floorVerts[3].UVs = { 1, 1, 0 };
	floorVerts[0].normals = { 0, 1, 0 };
	floorVerts[1].normals = { 0, 1, 0 };
	floorVerts[2].normals = { 0, 1, 0 };
	floorVerts[3].normals = { 0, 1, 0 };

	D3D11_BUFFER_DESC FloorBufferDesc;
	memset(&FloorBufferDesc, 0, sizeof(D3D11_BUFFER_DESC));
	FloorBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	FloorBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	FloorBufferDesc.CPUAccessFlags = NULL;
	FloorBufferDesc.ByteWidth = sizeof(SIMPLE_VERTEX) * 4;

	D3D11_SUBRESOURCE_DATA FloorSubresource;
	memset(&FloorSubresource, 0, sizeof(FloorSubresource));
	FloorSubresource.pSysMem = floorVerts;

	pDevice->CreateBuffer(&FloorBufferDesc, &FloorSubresource, &FloorVertexBuffer);

	D3D11_BUFFER_DESC FloorIndexDesc;
	memset(&FloorIndexDesc, 0, sizeof(D3D11_BUFFER_DESC));
	FloorIndexDesc.Usage = D3D11_USAGE_IMMUTABLE;
	FloorIndexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	FloorIndexDesc.CPUAccessFlags = NULL;
	FloorIndexDesc.ByteWidth = sizeof(unsigned int) * 6;
	memset(&FloorSubresource, 0, sizeof(D3D11_SUBRESOURCE_DATA));
	unsigned int FIndices[6] =
	{
		0, 1, 2,
		2, 1, 3
	};
	FloorSubresource.pSysMem = FIndices;
	pDevice->CreateBuffer(&FloorIndexDesc, &FloorSubresource, &FloorIndexBuffer);

	//arc fighter
	vector<XMFLOAT3> ArcObjVerts;
	vector<XMFLOAT3> ArcObjUVs;
	vector<XMFLOAT3> ArcObjNorms;
	LoadOBJ("Assets/Model/ARCFIGHTER.obj", ArcObjVerts, ArcObjUVs, ArcObjNorms);
	SIMPLE_VERTEX *ArcFighter = new SIMPLE_VERTEX[ArcObjVerts.size()];
	ARCVertCount = ArcObjVerts.size();
	for (unsigned int i = 0; i < ArcObjVerts.size(); i++)
	{
		ArcFighter[i].pos = ArcObjVerts[i];
		ArcFighter[i].UVs = ArcObjUVs[i];
		ArcFighter[i].normals = ArcObjNorms[i];
	}
	D3D11_BUFFER_DESC ARCFighterBufferDesc;
	memset(&ARCFighterBufferDesc, 0, sizeof(D3D11_BUFFER_DESC));
	ARCFighterBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	ARCFighterBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	ARCFighterBufferDesc.CPUAccessFlags = NULL;
	ARCFighterBufferDesc.ByteWidth = sizeof(SIMPLE_VERTEX) * ARCVertCount;

	D3D11_SUBRESOURCE_DATA ARCFighterSubresource;
	memset(&ARCFighterSubresource, 0, sizeof(ARCFighterSubresource));
	ARCFighterSubresource.pSysMem = ArcFighter;
	pDevice->CreateBuffer(&ARCFighterBufferDesc, &ARCFighterSubresource, &pARCFighterVBuffer);

	delete ArcFighter;

	//create goomba
	vector<XMFLOAT3> GoombaObjVerts;
	vector<XMFLOAT3> GoombaObjUVs;
	vector<XMFLOAT3> GoombaObjNorms;
	LoadOBJ("Assets/Model/GOOMBA.obj", GoombaObjVerts, GoombaObjUVs, GoombaObjNorms);
	SIMPLE_VERTEX *CorruptGoomba = new SIMPLE_VERTEX[GoombaObjVerts.size()];
	GoombaVertCount = GoombaObjVerts.size();
	for (unsigned int i = 0; i < GoombaObjVerts.size(); i++)
	{
		CorruptGoomba[i].pos = GoombaObjVerts[i];
		CorruptGoomba[i].UVs = GoombaObjUVs[i];
		CorruptGoomba[i].normals = GoombaObjNorms[i];
	}
	D3D11_BUFFER_DESC GoombaBufferDesc;
	memset(&GoombaBufferDesc, 0, sizeof(D3D11_BUFFER_DESC));
	GoombaBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	GoombaBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	GoombaBufferDesc.CPUAccessFlags = NULL;
	GoombaBufferDesc.ByteWidth = sizeof(SIMPLE_VERTEX) * GoombaVertCount;

	D3D11_SUBRESOURCE_DATA GoombaSubresource;
	memset(&GoombaSubresource, 0, sizeof(GoombaSubresource));
	GoombaSubresource.pSysMem = CorruptGoomba;
	pDevice->CreateBuffer(&GoombaBufferDesc, &GoombaSubresource, &pGoombaVBuff);

	delete CorruptGoomba;

	//Triangle for GS
	SIMPLE_VERTEX TriangleVert;
	TriangleVert.pos = { 0.0f, 5.0f, -5.0f };


	D3D11_BUFFER_DESC triangle_desc;
	memset(&triangle_desc, 0, sizeof(D3D11_BUFFER_DESC));
	triangle_desc.Usage = D3D11_USAGE_IMMUTABLE;
	triangle_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	triangle_desc.CPUAccessFlags = NULL;
	triangle_desc.ByteWidth = sizeof(SIMPLE_VERTEX);

	D3D11_SUBRESOURCE_DATA triangle_sr;
	memset(&triangle_sr, 0, sizeof(triangle_sr));
	triangle_sr.pSysMem = &TriangleVert;

	pDevice->CreateBuffer(&triangle_desc, &triangle_sr, &GSVertexBuffer);

	//Instanced GS
	SIMPLE_VERTEX InstancedCubes[3];
	InstancedCubes[0].pos = { 0, 0.5f, -4.5f };
	InstancedCubes[1].pos = { 3, 0.5f, -4.5f };
	InstancedCubes[2].pos = { -3, 0.5f, -4.5f };

	D3D11_BUFFER_DESC instanced_desc;
	memset(&instanced_desc, 0, sizeof(D3D11_BUFFER_DESC));
	instanced_desc.Usage = D3D11_USAGE_IMMUTABLE;
	instanced_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanced_desc.CPUAccessFlags = NULL;
	instanced_desc.ByteWidth = sizeof(SIMPLE_VERTEX) * 3;

	D3D11_SUBRESOURCE_DATA instanced_sr;
	memset(&instanced_sr, 0, sizeof(instanced_sr));
	instanced_sr.pSysMem = &InstancedCubes;

	pDevice->CreateBuffer(&instanced_desc, &instanced_sr, &InstancedVBuff);

	//Viewport Display
	SIMPLE_VERTEX ViewportPlain;
	ViewportPlain.pos = { 0, 0, 0 };
	D3D11_BUFFER_DESC vp_desc;
	memset(&vp_desc, 0, sizeof(D3D11_BUFFER_DESC));
	vp_desc.Usage = D3D11_USAGE_IMMUTABLE;
	vp_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vp_desc.CPUAccessFlags = NULL;
	vp_desc.ByteWidth = sizeof(SIMPLE_VERTEX);

	D3D11_SUBRESOURCE_DATA vp_sr;
	memset(&vp_sr, 0, sizeof(vp_sr));
	vp_sr.pSysMem = &ViewportPlain;

	pDevice->CreateBuffer(&vp_desc, &vp_sr, &ViewportPlainVBuff);

	//ZBuffer
	D3D11_TEXTURE2D_DESC text2D_Desc;
	memset(&text2D_Desc, 0, sizeof(D3D11_TEXTURE2D_DESC));
	text2D_Desc.Width = BACKBUFFER_WIDTH;
	text2D_Desc.Height = BACKBUFFER_HEIGHT;
	text2D_Desc.MipLevels = 1;
	text2D_Desc.ArraySize = 1;
	text2D_Desc.SampleDesc.Count = 1;
	text2D_Desc.SampleDesc.Quality = 0;
	text2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	text2D_Desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	text2D_Desc.Format = DXGI_FORMAT_D32_FLOAT;
	pDevice->CreateTexture2D(&text2D_Desc, NULL, &ZBuffer);
	D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc;
	memset(&DSVDesc, 0, sizeof(DSVDesc));
	DSVDesc.Format = DXGI_FORMAT_D32_FLOAT;
	DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	pDevice->CreateDepthStencilView(ZBuffer, &DSVDesc, &DPV);

	//shaders
	pDevice->CreateVertexShader(&Trivial_VS, sizeof(Trivial_VS), NULL, &pObjectVS);
	pDevice->CreatePixelShader(&Trivial_PS, sizeof(Trivial_PS), NULL, &pLightingPS);
	pDevice->CreateVertexShader(&SkyboxVertexShader, sizeof(SkyboxVertexShader), NULL, &SkyboxVS);
	pDevice->CreatePixelShader(&SkyboxPixelShader, sizeof(SkyboxPixelShader), NULL, &SkyboxPS);
	pDevice->CreatePixelShader(&MiniGrid_PS, sizeof(MiniGrid_PS), NULL, &GSPixelShader);
	pDevice->CreateGeometryShader(&MiniGrid_GS, sizeof(MiniGrid_GS), NULL, &TriangleGS);
	pDevice->CreateGeometryShader(&BillboardGS, sizeof(BillboardGS), NULL, &SecondViewGS);
	pDevice->CreateVertexShader(&GeomtryVS, sizeof(GeomtryVS), NULL, &TriangleVS);
	pDevice->CreateGeometryShader(&InstancedExampleGS, sizeof(InstancedExampleGS), NULL, &InstancedGS);
	pDevice->CreatePixelShader(&SecondViewportPS, sizeof(SecondViewportPS), NULL, &SecondViewPS);
	pDevice->CreateVertexShader(&InstanceVShader, sizeof(InstanceVShader), NULL, &VS_Instanced);

	//inputlayout
	D3D11_INPUT_ELEMENT_DESC vLayout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UVS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMALS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	pDevice->CreateInputLayout(vLayout, 3, &Trivial_VS, sizeof(Trivial_VS), &InputLayout);

	//constant buffers
	D3D11_BUFFER_DESC AnotherBufferTemp;
	memset(&AnotherBufferTemp, 0, sizeof(D3D11_BUFFER_DESC));
	AnotherBufferTemp.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	AnotherBufferTemp.Usage = D3D11_USAGE_DYNAMIC;
	AnotherBufferTemp.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	XMMATRIX tempMatrix;
	//floor
	tempMatrix = XMMatrixIdentity();
	tempMatrix *= XMMatrixTranslation(0, 0, 0);
	XMStoreFloat4x4(&FloorWorld.worldMatrix, tempMatrix);
	AnotherBufferTemp.ByteWidth = sizeof(SEND_WORLD_MATRIX);
	AnotherBufferTemp.StructureByteStride = sizeof(SEND_WORLD_MATRIX);
	pDevice->CreateBuffer(&AnotherBufferTemp, NULL, &FloorConstBuffer);

	//plain view
	tempMatrix = XMMatrixIdentity();
	XMStoreFloat4x4(&FloorWorld.worldMatrix, tempMatrix);
	AnotherBufferTemp.ByteWidth = sizeof(SEND_WORLD_MATRIX);
	AnotherBufferTemp.StructureByteStride = sizeof(SEND_WORLD_MATRIX);
	pDevice->CreateBuffer(&AnotherBufferTemp, NULL, &ViewportConstBuff);

	//backdrop
	tempMatrix = XMMatrixIdentity();
	tempMatrix *= XMMatrixRotationX(XMConvertToRadians(-90.0f));
	tempMatrix *= XMMatrixTranslation(0, 5.0f, 5.0f);
	XMStoreFloat4x4(&BackDrop.worldMatrix, tempMatrix);

	//skybox
	tempMatrix = XMMatrixIdentity();
	XMStoreFloat4x4(&SkyboxWorld.worldMatrix, tempMatrix);
	XMStoreFloat4x4(&SkyboxWorldForSecondViewport.worldMatrix, tempMatrix);
	AnotherBufferTemp.ByteWidth = sizeof(SEND_WORLD_MATRIX);
	AnotherBufferTemp.StructureByteStride = sizeof(SEND_WORLD_MATRIX);
	pDevice->CreateBuffer(&AnotherBufferTemp, NULL, &SkyboxConstBuffer);

	//ARC170 
	tempMatrix = XMMatrixIdentity();
	tempMatrix *= XMMatrixScaling(0.005f, 0.005f, 0.005f);
	tempMatrix *= XMMatrixTranslation(0, 3, 0);
	XMStoreFloat4x4(&ARC170World[0].worldMatrix, tempMatrix);

	tempMatrix = XMMatrixIdentity();
	tempMatrix *= XMMatrixScaling(0.005f, 0.005f, 0.005f);
	tempMatrix *= XMMatrixTranslation(9, 3, 0);
	tempMatrix = XMMatrixRotationY(XMConvertToRadians(45.0f)) * tempMatrix;
	XMStoreFloat4x4(&ARC170World[1].worldMatrix, tempMatrix);

	tempMatrix = XMMatrixIdentity();
	tempMatrix *= XMMatrixScaling(0.005f, 0.005f, 0.005f);
	tempMatrix *= XMMatrixTranslation(-9, 3, 0);
	tempMatrix = XMMatrixRotationY(XMConvertToRadians(-45.0f)) * tempMatrix;
	XMStoreFloat4x4(&ARC170World[2].worldMatrix, tempMatrix);

	AnotherBufferTemp.ByteWidth = sizeof(SEND_WORLD_MATRIX) * 3;
	AnotherBufferTemp.StructureByteStride = sizeof(SEND_WORLD_MATRIX);
	pDevice->CreateBuffer(&AnotherBufferTemp, NULL, &ARCConstBuffer);

	//Goomba
	tempMatrix = XMMatrixIdentity();
	tempMatrix *= XMMatrixTranslation(0, 0, 0);
	XMStoreFloat4x4(&GoombaWorld.worldMatrix, tempMatrix);
	AnotherBufferTemp.ByteWidth = sizeof(SEND_WORLD_MATRIX);
	AnotherBufferTemp.StructureByteStride = sizeof(SEND_WORLD_MATRIX);
	pDevice->CreateBuffer(&AnotherBufferTemp, NULL, &GoombaConstBuff);
	GoombaPostion = { 0, 0, 0, 1 };

	//Instanced
	tempMatrix = XMMatrixIdentity();
	XMStoreFloat4x4(&InstancedWorld.worldMatrix, tempMatrix);

	//triangle 
	tempMatrix = XMMatrixIdentity();
	XMStoreFloat4x4(&TriangleWorld.worldMatrix, tempMatrix);
	AnotherBufferTemp.ByteWidth = sizeof(SEND_WORLD_MATRIX);
	AnotherBufferTemp.StructureByteStride = sizeof(SEND_WORLD_MATRIX);
	pDevice->CreateBuffer(&AnotherBufferTemp, NULL, &GSConstBuffer);

	//perspective
	tempMatrix = XMMatrixIdentity();
	tempMatrix = XMMatrixTranslation(0, -3, 15);
	XMStoreFloat4x4(&sceneMatrices.viewMatrix, tempMatrix);
	XMStoreFloat4x4(&sceneMatrices.projectionMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(75.0f), BACKBUFFER_WIDTH / BACKBUFFER_HEIGHT, 0.1f, 100.0f));
	AnotherBufferTemp.ByteWidth = sizeof(SEND_SCENE_MATRICES);
	AnotherBufferTemp.StructureByteStride = sizeof(SEND_SCENE_MATRICES);
	pDevice->CreateBuffer(&AnotherBufferTemp, NULL, &ConstantBufferToSendScene);

	//offscreen
	tempMatrix = XMMatrixIdentity();
	tempMatrix *= XMMatrixTranslation(0, 0, 5);
	XMStoreFloat4x4(&OffscreenMatrices.viewMatrix, tempMatrix);
	XMStoreFloat4x4(&OffscreenMatrices.projectionMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(75.0f), 1, 0.1f, 100.0f));

	//second view;
	tempMatrix = XMMatrixIdentity();
	tempMatrix *= XMMatrixTranslation(0, -3, 15);
	XMStoreFloat4x4(&SecondViewMatrices.viewMatrix, tempMatrix);
	XMStoreFloat4x4(&SecondViewMatrices.projectionMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(75.0f), BACKBUFFER_WIDTH / BACKBUFFER_HEIGHT, 0.1f, 100.0f));

	//directional light
	AnotherBufferTemp.ByteWidth = sizeof(DIRECTIONAL_LIGHT);
	AnotherBufferTemp.StructureByteStride = sizeof(DIRECTIONAL_LIGHT);
	pDevice->CreateBuffer(&AnotherBufferTemp, NULL, &DirectLightConstBuff);
	directLight.direction = { 1, -1, 1, 0 };
	directLight.color = { 1, 1, 1, 1 };

	//point light
	AnotherBufferTemp.ByteWidth = sizeof(POINT_LIGHT);
	AnotherBufferTemp.StructureByteStride = sizeof(POINT_LIGHT);
	pDevice->CreateBuffer(&AnotherBufferTemp, NULL, &PointLightConstBuff);

	pointLight.position = { 4, 1, -4 };
	PLPosition = { 4, 1, -4, 1 };
	pointLight.color = { 1, 0.4f, 0, 1 };
	pointLight.radius = 25.0f;

	//spot light
	AnotherBufferTemp.ByteWidth = sizeof(SPOT_LIGHT);
	AnotherBufferTemp.StructureByteStride = sizeof(SPOT_LIGHT);
	pDevice->CreateBuffer(&AnotherBufferTemp, NULL, &SpotLightConstBuff);

	spotLight.position = { 0, 8, 0 };
	spotLight.ConeDir = { 0, -1, 0 };
	spotLight.radius = 15.0f;
	spotLight.innerRatio = 0.98f;
	spotLight.outerRatio = 0.9f;
	spotLight.color = { 0, 1, 1, 1 };

	//multitextures branching buffers
	AnotherBufferTemp.ByteWidth = sizeof(MULTITEXTURE_BOOL);
	AnotherBufferTemp.StructureByteStride = sizeof(MULTITEXTURE_BOOL);
	pDevice->CreateBuffer(&AnotherBufferTemp, NULL, &MultiTextureConstBuff);

	WeHaveMultiTextures.multiple = { 1, 0, 0, 0 };
	NoMultiTextures.multiple = { 0, 1, 0, 0 };

	//mirage const buff
	AnotherBufferTemp.ByteWidth = sizeof(TIME_HERE);
	AnotherBufferTemp.StructureByteStride = sizeof(TIME_HERE);
	pDevice->CreateBuffer(&AnotherBufferTemp, NULL, &MirageConstBuff);


	ttime = new XTime();

	pCommandList = NULL;
	RunningThread = true;

	CreateDDSTextureFromFile(pDevice, L"Assets/Textures/DuneSkybox.dds", nullptr, &pSkyboxSRV);
	CreateDDSTextureFromFile(pDevice, L"Assets/Textures/ARC170_TEXTURE.dds", nullptr, &pARCFighterSRV);
	CreateDDSTextureFromFile(pDevice, L"Assets/Textures/CORRUPTGOOMBA.dds", nullptr, &GoombaSRV);
	CreateDDSTextureFromFile(pDevice, L"Assets/Textures/frame.dds", nullptr, &FrameSRV);
	CreateDDSTextureFromFile(pDevice, L"Assets/Textures/PurpBox.dds", nullptr, &InstancedSRV);
	CreateDDSTextureFromFile(pDevice, L"Assets/Textures/Crosshair.dds", nullptr, &MultiTextureProof);
	
	thred = thread(BIGLOAD, this, L"Assets/Textures/TiledFloor.dds");

	velocity = PLVelocity = SLVelocity = { 5.0f, 5.0f };
	
	HRESULT hr = pDevice->QueryInterface(IID_PPV_ARGS(&pDebug));
}

//************************************************************
//************ EXECUTION *************************************
//************************************************************

bool DEMO_APP::Run()
{
	ttime->Signal();

	MoveCamera();
	MoveSkybox();
	MoveGoomba(ttime->Delta());
	MoveLights(ttime->Delta());

	D3D11_MAPPED_SUBRESOURCE MapSubresource;
	unsigned int VertexSize = sizeof(SIMPLE_VERTEX);
	unsigned int Array[] = { 0 };
	float Colour[4] = { 200.0f / 255.0f, 0, 220.0f / 225.0f, 255.0f / 255.0f };

	TheTime.time.x = ttime->TotalTime();

	if (pCommandList.load() != nullptr)
	{
		pDeviceContext->ClearState();

		pDeviceContext->RSSetViewports(1, SecondViewport);
		pDeviceContext->OMSetRenderTargets(1, &SecondViewRTV.p, SecondViewDPV);
		pDeviceContext->ClearRenderTargetView(SecondViewRTV, Colour);
		pDeviceContext->ClearDepthStencilView(SecondViewDPV, D3D11_CLEAR_DEPTH, 1, NULL);

		//skybox
		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(SkyboxConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &SkyboxWorldForSecondViewport, sizeof(SkyboxWorldForSecondViewport));
		pDeviceContext->Unmap(SkyboxConstBuffer, 0);
		
		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(ConstantBufferToSendScene, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &SecondViewMatrices, sizeof(SecondViewMatrices));
		pDeviceContext->Unmap(ConstantBufferToSendScene, 0);

		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(MirageConstBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &TheTime, sizeof(TheTime));
		pDeviceContext->Unmap(MirageConstBuff, 0);
		
		pDeviceContext->VSSetConstantBuffers(0, 1, &SkyboxConstBuffer.p);
		pDeviceContext->VSSetConstantBuffers(1, 1, &ConstantBufferToSendScene.p);
		pDeviceContext->PSSetConstantBuffers(0, 1, &MirageConstBuff.p);
		pDeviceContext->PSSetShaderResources(0, 1, &pSkyboxSRV.p);
		pDeviceContext->PSSetSamplers(0, 1, &SkyboxSampler.p);
		
		pDeviceContext->IASetVertexBuffers(0, 1, &pSkyboxVBuffer.p, &VertexSize, Array);
		pDeviceContext->IASetIndexBuffer(pSkyboxIBuffer.p, DXGI_FORMAT_R32_UINT, 0);
		pDeviceContext->PSSetShader(SkyboxPS.p, NULL, 0);
		pDeviceContext->VSSetShader(SkyboxVS.p, NULL, 0);
		pDeviceContext->IASetInputLayout(InputLayout.p);
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pDeviceContext->DrawIndexed(36, 0, 0);
		pDeviceContext->ClearDepthStencilView(SecondViewDPV, D3D11_CLEAR_DEPTH, 1, NULL);

		//ARC FIGHTER Draw
		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(ARCConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &ARC170World, sizeof(ARC170World));
		pDeviceContext->Unmap(ARCConstBuffer, 0);

		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(ConstantBufferToSendScene, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &SecondViewMatrices, sizeof(SecondViewMatrices));
		pDeviceContext->Unmap(ConstantBufferToSendScene, 0);

		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(DirectLightConstBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &directLight, sizeof(directLight));
		pDeviceContext->Unmap(DirectLightConstBuff, 0);

		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(PointLightConstBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &pointLight, sizeof(pointLight));
		pDeviceContext->Unmap(PointLightConstBuff, 0);

		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(SpotLightConstBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &spotLight, sizeof(spotLight));
		pDeviceContext->Unmap(SpotLightConstBuff, 0);

		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(MultiTextureConstBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &NoMultiTextures, sizeof(NoMultiTextures));
		pDeviceContext->Unmap(MultiTextureConstBuff, 0);

		pDeviceContext->PSSetConstantBuffers(0, 1, &PointLightConstBuff.p);
		pDeviceContext->PSSetConstantBuffers(1, 1, &SpotLightConstBuff.p);
		pDeviceContext->PSSetConstantBuffers(2, 1, &DirectLightConstBuff.p);
		pDeviceContext->PSSetConstantBuffers(3, 1, &MultiTextureConstBuff.p);
		pDeviceContext->VSSetConstantBuffers(0, 1, &ARCConstBuffer.p);
		pDeviceContext->VSSetConstantBuffers(1, 1, &ConstantBufferToSendScene.p);
		pDeviceContext->PSSetShaderResources(0, 1, &pARCFighterSRV.p);
		pDeviceContext->PSSetSamplers(0, 1, &pARCFighterSampler.p);
		pDeviceContext->IASetVertexBuffers(0, 1, &pARCFighterVBuffer.p, &VertexSize, Array);
		pDeviceContext->PSSetShader(pLightingPS.p, NULL, 0);
		pDeviceContext->VSSetShader(VS_Instanced.p, NULL, 0);
		pDeviceContext->IASetInputLayout(InputLayout.p);
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pDeviceContext->DrawInstanced(ARCVertCount, 3, 0, 0);

		//backdrop
		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(FloorConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &BackDrop, sizeof(BackDrop));
		pDeviceContext->Unmap(FloorConstBuffer, 0);

		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(MultiTextureConstBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &NoMultiTextures, sizeof(NoMultiTextures));
		pDeviceContext->Unmap(MultiTextureConstBuff, 0);

		pDeviceContext->PSSetConstantBuffers(0, 1, &PointLightConstBuff.p);
		pDeviceContext->PSSetConstantBuffers(1, 1, &SpotLightConstBuff.p);
		pDeviceContext->PSSetConstantBuffers(2, 1, &DirectLightConstBuff.p);
		pDeviceContext->PSSetConstantBuffers(3, 1, &MultiTextureConstBuff.p);
		pDeviceContext->VSSetConstantBuffers(0, 1, &FloorConstBuffer.p);
		pDeviceContext->VSSetConstantBuffers(1, 1, &ConstantBufferToSendScene.p);
		pDeviceContext->PSSetShaderResources(0, 1, &OffscreenSRV.p);
		pDeviceContext->PSSetSamplers(0, 1, &pFloorSamplerState.p);
		pDeviceContext->IASetVertexBuffers(0, 1, &FloorVertexBuffer.p, &VertexSize, Array);
		pDeviceContext->IASetIndexBuffer(FloorIndexBuffer.p, DXGI_FORMAT_R32_UINT, 0);
		pDeviceContext->PSSetShader(pLightingPS.p, NULL, 0);
		pDeviceContext->VSSetShader(pObjectVS.p, NULL, 0);
		pDeviceContext->IASetInputLayout(InputLayout.p);
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pDeviceContext->DrawIndexed(6, 0, 0);

		//instanced draw
		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(GSConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &InstancedWorld, sizeof(InstancedWorld));
		pDeviceContext->Unmap(GSConstBuffer, 0);

		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(ConstantBufferToSendScene, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &SecondViewMatrices, sizeof(SecondViewMatrices));
		pDeviceContext->Unmap(ConstantBufferToSendScene, 0);

		pDeviceContext->PSSetShaderResources(0, 1, &InstancedSRV.p);
		pDeviceContext->PSSetSamplers(0, 1, &InstanceSampler.p);
		pDeviceContext->GSSetConstantBuffers(0, 1, &GSConstBuffer.p);
		pDeviceContext->GSSetConstantBuffers(1, 1, &ConstantBufferToSendScene.p);
		pDeviceContext->IASetVertexBuffers(0, 1, &InstancedVBuff.p, &VertexSize, Array);
		pDeviceContext->PSSetShader(GSPixelShader.p, NULL, 0);
		pDeviceContext->VSSetShader(TriangleVS.p, NULL, 0);
		pDeviceContext->GSSetShader(InstancedGS.p, NULL, 0);
		pDeviceContext->IASetInputLayout(InputLayout.p);
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		pDeviceContext->Draw(3, 0);
		pDeviceContext->GSSetShader(nullptr, NULL, 0);

		//triangle draw
		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(GSConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &TriangleWorld, sizeof(TriangleWorld));
		pDeviceContext->Unmap(GSConstBuffer, 0);

		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(ConstantBufferToSendScene, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &SecondViewMatrices, sizeof(SecondViewMatrices));
		pDeviceContext->Unmap(ConstantBufferToSendScene, 0);

		pDeviceContext->PSSetShaderResources(0, 1, &FrameSRV.p);
		pDeviceContext->PSSetSamplers(0, 1, &FrameSamplerState.p);
		pDeviceContext->GSSetConstantBuffers(0, 1, &GSConstBuffer.p);
		pDeviceContext->GSSetConstantBuffers(1, 1, &ConstantBufferToSendScene.p);
		pDeviceContext->IASetVertexBuffers(0, 1, &GSVertexBuffer.p, &VertexSize, Array);
		pDeviceContext->PSSetShader(GSPixelShader.p, NULL, 0);
		pDeviceContext->VSSetShader(TriangleVS.p, NULL, 0);
		pDeviceContext->GSSetShader(TriangleGS.p, NULL, 0);
		pDeviceContext->IASetInputLayout(InputLayout.p);
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		pDeviceContext->Draw(1, 0);
		pDeviceContext->GSSetShader(nullptr, NULL, 0);


		pDeviceContext->ClearState();
		//Goomba Draw
		pDeviceContext->RSSetViewports(1, OffScreenViewPort);
		pDeviceContext->OMSetRenderTargets(1, &GoombaRTV.p, GoombaDPV);
		pDeviceContext->ClearRenderTargetView(GoombaRTV, Colour);
		pDeviceContext->ClearDepthStencilView(GoombaDPV, D3D11_CLEAR_DEPTH, 1, NULL);

		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(GoombaConstBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &GoombaWorld, sizeof(GoombaWorld));
		pDeviceContext->Unmap(GoombaConstBuff, 0);

		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(ConstantBufferToSendScene, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &OffscreenMatrices, sizeof(OffscreenMatrices));
		pDeviceContext->Unmap(ConstantBufferToSendScene, 0);

		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(DirectLightConstBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &directLight, sizeof(directLight));
		pDeviceContext->Unmap(DirectLightConstBuff, 0);

		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(PointLightConstBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &pointLight, sizeof(pointLight));
		pDeviceContext->Unmap(PointLightConstBuff, 0);

		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(SpotLightConstBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &spotLight, sizeof(spotLight));
		pDeviceContext->Unmap(SpotLightConstBuff, 0);

		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(MultiTextureConstBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &NoMultiTextures, sizeof(NoMultiTextures));
		pDeviceContext->Unmap(MultiTextureConstBuff, 0);

		pDeviceContext->PSSetConstantBuffers(0, 1, &PointLightConstBuff.p);
		pDeviceContext->PSSetConstantBuffers(1, 1, &SpotLightConstBuff.p);
		pDeviceContext->PSSetConstantBuffers(2, 1, &DirectLightConstBuff.p);
		pDeviceContext->PSSetConstantBuffers(3, 1, &MultiTextureConstBuff.p);
		pDeviceContext->VSSetConstantBuffers(0, 1, &GoombaConstBuff.p);
		pDeviceContext->VSSetConstantBuffers(1, 1, &ConstantBufferToSendScene.p);
		pDeviceContext->PSSetShaderResources(0, 1, &GoombaSRV.p);
		pDeviceContext->PSSetSamplers(0, 1, &pGoombaSampler.p);
		pDeviceContext->IASetVertexBuffers(0, 1, &pGoombaVBuff.p, &VertexSize, Array);
		pDeviceContext->PSSetShader(pLightingPS.p, NULL, 0);
		pDeviceContext->VSSetShader(pObjectVS.p, NULL, 0);
		pDeviceContext->IASetInputLayout(InputLayout.p);
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pDeviceContext->Draw(GoombaVertCount, 0);
		pDeviceContext->GenerateMips(OffscreenSRV.p);

		//Drawing the actually scene
		pDeviceContext->ClearState();
		pDeviceContext->RSSetViewports(1, viewport);
		pDeviceContext->OMSetRenderTargets(1, &RTV.p, DPV);
		pDeviceContext->ClearRenderTargetView(RTV, Colour);
		pDeviceContext->ClearDepthStencilView(DPV, D3D11_CLEAR_DEPTH, 1, NULL);

		//skybox
		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(SkyboxConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &SkyboxWorld, sizeof(SkyboxWorld));
		pDeviceContext->Unmap(SkyboxConstBuffer, 0);

		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(ConstantBufferToSendScene, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &sceneMatrices, sizeof(sceneMatrices));
		pDeviceContext->Unmap(ConstantBufferToSendScene, 0);

		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(MirageConstBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &TheTime, sizeof(TheTime));
		pDeviceContext->Unmap(MirageConstBuff, 0);

		pDeviceContext->VSSetConstantBuffers(0, 1, &SkyboxConstBuffer.p);
		pDeviceContext->VSSetConstantBuffers(1, 1, &ConstantBufferToSendScene.p);
		pDeviceContext->PSSetConstantBuffers(0, 1, &MirageConstBuff.p);
		pDeviceContext->PSSetShaderResources(0, 1, &pSkyboxSRV.p);
		pDeviceContext->PSSetSamplers(0, 1, &SkyboxSampler.p);

		pDeviceContext->IASetVertexBuffers(0, 1, &pSkyboxVBuffer.p, &VertexSize, Array);
		pDeviceContext->IASetIndexBuffer(pSkyboxIBuffer.p, DXGI_FORMAT_R32_UINT, 0);
		pDeviceContext->PSSetShader(SkyboxPS.p, NULL, 0);
		pDeviceContext->VSSetShader(SkyboxVS.p, NULL, 0);
		pDeviceContext->IASetInputLayout(InputLayout.p);
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pDeviceContext->DrawIndexed(36, 0, 0);
		pDeviceContext->ClearDepthStencilView(DPV, D3D11_CLEAR_DEPTH, 1, NULL);

		//command list
		pDeviceContext->ExecuteCommandList(pCommandList, false);
		pCommandList.load()->Release();
		pCommandList = nullptr;


		pDeviceContext->ClearState();
		pDeviceContext->RSSetViewports(1, viewport);
		pDeviceContext->OMSetRenderTargets(1, &RTV.p, DPV);

		//BackDrop
		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(FloorConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &BackDrop, sizeof(BackDrop));
		pDeviceContext->Unmap(FloorConstBuffer, 0);

		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(MultiTextureConstBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &NoMultiTextures, sizeof(NoMultiTextures));
		pDeviceContext->Unmap(MultiTextureConstBuff, 0);

		pDeviceContext->PSSetConstantBuffers(0, 1, &PointLightConstBuff.p);
		pDeviceContext->PSSetConstantBuffers(1, 1, &SpotLightConstBuff.p);
		pDeviceContext->PSSetConstantBuffers(2, 1, &DirectLightConstBuff.p);
		pDeviceContext->PSSetConstantBuffers(3, 1, &MultiTextureConstBuff.p);
		pDeviceContext->VSSetConstantBuffers(0, 1, &FloorConstBuffer.p);
		pDeviceContext->VSSetConstantBuffers(1, 1, &ConstantBufferToSendScene.p);
		pDeviceContext->PSSetShaderResources(0, 1, &OffscreenSRV.p);
		pDeviceContext->PSSetSamplers(0, 1, &pFloorSamplerState.p);
		pDeviceContext->IASetVertexBuffers(0, 1, &FloorVertexBuffer.p, &VertexSize, Array);
		pDeviceContext->IASetIndexBuffer(FloorIndexBuffer.p, DXGI_FORMAT_R32_UINT, 0);
		pDeviceContext->PSSetShader(pLightingPS.p, NULL, 0);
		pDeviceContext->VSSetShader(pObjectVS.p, NULL, 0);
		pDeviceContext->IASetInputLayout(InputLayout.p);
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pDeviceContext->DrawIndexed(6, 0, 0);

		//ARC FIGHTER Draw
		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(ARCConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &ARC170World, sizeof(ARC170World));
		pDeviceContext->Unmap(ARCConstBuffer, 0);

		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(ConstantBufferToSendScene, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &sceneMatrices, sizeof(sceneMatrices));
		pDeviceContext->Unmap(ConstantBufferToSendScene, 0);

		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(DirectLightConstBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &directLight, sizeof(directLight));
		pDeviceContext->Unmap(DirectLightConstBuff, 0);

		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(PointLightConstBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &pointLight, sizeof(pointLight));
		pDeviceContext->Unmap(PointLightConstBuff, 0);

		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(SpotLightConstBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &spotLight, sizeof(spotLight));
		pDeviceContext->Unmap(SpotLightConstBuff, 0);

		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(MultiTextureConstBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &NoMultiTextures, sizeof(NoMultiTextures));
		pDeviceContext->Unmap(MultiTextureConstBuff, 0);

		pDeviceContext->PSSetConstantBuffers(0, 1, &PointLightConstBuff.p);
		pDeviceContext->PSSetConstantBuffers(1, 1, &SpotLightConstBuff.p);
		pDeviceContext->PSSetConstantBuffers(2, 1, &DirectLightConstBuff.p);
		pDeviceContext->PSSetConstantBuffers(3, 1, &MultiTextureConstBuff.p);
		pDeviceContext->VSSetConstantBuffers(0, 1, &ARCConstBuffer.p);
		pDeviceContext->VSSetConstantBuffers(1, 1, &ConstantBufferToSendScene.p);
		pDeviceContext->PSSetShaderResources(0, 1, &pARCFighterSRV.p);
		pDeviceContext->PSSetSamplers(0, 1, &pARCFighterSampler.p);
		pDeviceContext->IASetVertexBuffers(0, 1, &pARCFighterVBuffer.p, &VertexSize, Array);
		pDeviceContext->PSSetShader(pLightingPS.p, NULL, 0);
		pDeviceContext->VSSetShader(VS_Instanced.p, NULL, 0);
		pDeviceContext->IASetInputLayout(InputLayout.p);
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//pDeviceContext->Draw(ARCVertCount, 0);
		pDeviceContext->DrawInstanced(ARCVertCount, 3, 0, 0);

		//triangle draw
		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(GSConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &TriangleWorld, sizeof(TriangleWorld));
		pDeviceContext->Unmap(GSConstBuffer, 0);

		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(ConstantBufferToSendScene, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &sceneMatrices, sizeof(sceneMatrices));
		pDeviceContext->Unmap(ConstantBufferToSendScene, 0);

		pDeviceContext->PSSetShaderResources(0, 1, &FrameSRV.p);
		pDeviceContext->PSSetSamplers(0, 1, &FrameSamplerState.p);
		pDeviceContext->GSSetConstantBuffers(0, 1, &GSConstBuffer.p);
		pDeviceContext->GSSetConstantBuffers(1, 1, &ConstantBufferToSendScene.p);
		pDeviceContext->IASetVertexBuffers(0, 1, &GSVertexBuffer.p, &VertexSize, Array);
		pDeviceContext->PSSetShader(GSPixelShader.p, NULL, 0);
		pDeviceContext->VSSetShader(TriangleVS.p, NULL, 0);
		pDeviceContext->GSSetShader(TriangleGS.p, NULL, 0);
		pDeviceContext->IASetInputLayout(InputLayout.p);
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		pDeviceContext->Draw(1, 0);
		pDeviceContext->GSSetShader(nullptr, NULL, 0);

		//instanced draw
		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(GSConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &InstancedWorld, sizeof(InstancedWorld));
		pDeviceContext->Unmap(GSConstBuffer, 0);

		memset(&MapSubresource, 0, sizeof(MapSubresource));
		pDeviceContext->Map(ConstantBufferToSendScene, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
		memcpy(MapSubresource.pData, &sceneMatrices, sizeof(sceneMatrices));
		pDeviceContext->Unmap(ConstantBufferToSendScene, 0);

		pDeviceContext->PSSetShaderResources(0, 1, &InstancedSRV.p);
		pDeviceContext->PSSetSamplers(0, 1, &InstanceSampler.p);
		pDeviceContext->GSSetConstantBuffers(0, 1, &GSConstBuffer.p);
		pDeviceContext->GSSetConstantBuffers(1, 1, &ConstantBufferToSendScene.p);
		pDeviceContext->IASetVertexBuffers(0, 1, &InstancedVBuff.p, &VertexSize, Array);
		pDeviceContext->PSSetShader(GSPixelShader.p, NULL, 0);
		pDeviceContext->VSSetShader(TriangleVS.p, NULL, 0);
		pDeviceContext->GSSetShader(InstancedGS.p, NULL, 0);
		pDeviceContext->IASetInputLayout(InputLayout.p);
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		pDeviceContext->Draw(3, 0);
		pDeviceContext->GSSetShader(nullptr, NULL, 0);

		//viewport display
		pDeviceContext->PSSetShaderResources(0, 1, &SecondViewSRV.p);
		pDeviceContext->PSSetSamplers(0, 1, &FrameSamplerState.p);
		pDeviceContext->IASetVertexBuffers(0, 1, &ViewportPlainVBuff.p, &VertexSize, Array);
		pDeviceContext->PSSetShader(SecondViewPS.p, NULL, 0);
		pDeviceContext->VSSetShader(TriangleVS.p, NULL, 0);
		pDeviceContext->GSSetShader(SecondViewGS.p, NULL, 0);
		pDeviceContext->IASetInputLayout(InputLayout.p);
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		pDeviceContext->Draw(1, 0);
		pDeviceContext->GSSetShader(nullptr, NULL, 0);
		
		pSwapChain->Present(0, 0);
	}
	return true;
}

//************************************************************
//************ DESTRUCTION ***********************************
//************************************************************

bool DEMO_APP::ShutDown()
{
	(pDeviceContext)->ClearState();

	RunningThread = false;
	if (thred.joinable())
	{
		thred.join();
	}

	if (pDebug != nullptr)
	{
		pDebug.p->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		pDebug = nullptr;
	}

	delete ttime;
	delete viewport;
	delete OffScreenViewPort;
	delete SecondViewport;
	UnregisterClass(L"DirectXApplication", application);
	return true;
}

//************************************************************
//************ WINDOWS RELATED *******************************
//************************************************************

// ****************** BEGIN WARNING ***********************// 
// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY!w ssdsw a

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam);
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int)
{
	srand(unsigned int(time(0)));
	DEMO_APP myApp(hInstance, (WNDPROC)WndProc);
	MSG msg; ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT && myApp.Run())
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	myApp.ShutDown();
	return 0;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (GetAsyncKeyState(VK_ESCAPE))
		message = WM_DESTROY;
	switch (message)
	{
	case (WM_DESTROY) : { PostQuitMessage(0); }
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
//********************* END WARNING ************************//


void DEMO_APP::MoveSkybox()
{
	XMMATRIX Cam = XMMatrixInverse(nullptr, XMLoadFloat4x4(&sceneMatrices.viewMatrix));
	XMFLOAT4X4 tehCam, tehSkybox;
	XMStoreFloat4x4(&tehCam, Cam);
	tehSkybox = SkyboxWorld.worldMatrix;

	tehSkybox._41 = tehCam._41;
	tehSkybox._42 = tehCam._42;
	tehSkybox._43 = tehCam._43;

	SkyboxWorld.worldMatrix = tehSkybox;

	//second view port skybox
	XMMATRIX tehCam2 = XMMatrixInverse(nullptr, XMLoadFloat4x4(&SecondViewMatrices.viewMatrix));
	XMFLOAT4X4 cam2, sky2;
	XMStoreFloat4x4(&cam2, tehCam2);
	sky2 = SkyboxWorldForSecondViewport.worldMatrix;

	sky2._41 = cam2._41;
	sky2._42 = cam2._42;
	sky2._43 = cam2._43;

	SkyboxWorldForSecondViewport.worldMatrix = sky2;
}
void DEMO_APP::MoveCamera()
{
	XMMATRIX camera;
	camera = XMLoadFloat4x4(&sceneMatrices.viewMatrix);
	camera = XMMatrixInverse(NULL, camera);
	if (GetAsyncKeyState('W'))
	{//forward
		camera = DirectX::XMMatrixTranslation(0, 0, 25 * (float)ttime->Delta()) * camera;
	}
	if (GetAsyncKeyState('S'))
	{
		camera = DirectX::XMMatrixTranslation(0, 0, -25 * (float)ttime->Delta()) * camera;
	}
	if (GetAsyncKeyState('A'))
	{
		camera = DirectX::XMMatrixTranslation(-25 * (float)ttime->Delta(), 0, 0) * camera;
	}
	if (GetAsyncKeyState('D'))
	{
		camera = DirectX::XMMatrixTranslation(25 * (float)ttime->Delta(), 0, 0) * camera;
	}
	if (GetAsyncKeyState(VK_SPACE))
	{
		camera *= DirectX::XMMatrixTranslation(0, 25 * (float)ttime->Delta(), 0);
	}
	if (GetAsyncKeyState(VK_SHIFT))
	{
		camera *= DirectX::XMMatrixTranslation(0, -25 * (float)ttime->Delta(), 0);
	}

	POINT cur_point;
	GetCursorPos(&cur_point);
	static POINT prev_point = cur_point;
	float mouseX_diff = (float)(cur_point.x - prev_point.x);
	float mouseY_diff = (float)(cur_point.y - prev_point.y);
	if (GetAsyncKeyState(VK_RBUTTON))
	{
		XMMATRIX cam = camera;
		XMVECTOR pos = cam.r[3];
		cam.r[3] = g_XMZero;
		cam *= XMMatrixRotationY(mouseX_diff * 0.01f);
		cam *= XMMatrixRotationX(XMVectorGetX(cam.r[0]) * mouseY_diff * 0.01f) *
			XMMatrixRotationY(XMVectorGetY(cam.r[0]) * mouseY_diff * 0.01f) *
			XMMatrixRotationZ(XMVectorGetZ(cam.r[0]) * mouseY_diff * 0.01f);
		cam.r[3] = pos;
		camera = cam;
	}
	prev_point = cur_point;

	camera = XMMatrixInverse(NULL, camera);
	XMStoreFloat4x4(&sceneMatrices.viewMatrix, camera);




}
void DEMO_APP::MoveGoomba(double timer)
{
	XMMATRIX theMatrix;
	rotation += 35.0f * (float)timer;

	if (GoombaPostion.x < -6.0f || GoombaPostion.x > 6.0f)
	{
		velocity.x = -velocity.x;
	}
	if (GoombaPostion.y < -5.0f || GoombaPostion.y > 5.0f)
	{
		velocity.y = -velocity.y;
	}

	theMatrix = DirectX::XMMatrixRotationY(XMConvertToRadians(rotation));
	theMatrix = theMatrix * DirectX::XMMatrixRotationZ(XMConvertToRadians(rotation));
	
	float temp = velocity.x * (float)timer;
	GoombaPostion.x += velocity.x * (float)timer;
	GoombaPostion.y += velocity.y * (float)timer;
	theMatrix *= XMMatrixTranslation(GoombaPostion.x, GoombaPostion.y, 0);

	
	XMStoreFloat4x4(&GoombaWorld.worldMatrix, theMatrix);
}
void DEMO_APP::MoveLights(double timer)
{
	if (GetAsyncKeyState(VK_NUMPAD0) & 0x1) // flip directional light's y
	{
		directLight.direction.y = -directLight.direction.y;
	}


	if (pointLight.position.x < -5 || pointLight.position.x > 5)
	{
		PLVelocity.x = -PLVelocity.x;
	}
	if (pointLight.position.z < -5 || pointLight.position.z > 5)
	{
		PLVelocity.y = -PLVelocity.y;
	}

	pointLight.position.x += PLVelocity.x * (float)timer;
	pointLight.position.z += PLVelocity.y * (float)timer;

	//move spot light
	if (GetAsyncKeyState(VK_NUMPAD4) & 0x1)
	{
		spotLight.ConeDir = { -1, -1, 0 };
	}
	else if (GetAsyncKeyState(VK_NUMPAD6) & 0x1)
	{
		spotLight.ConeDir = { 1, -1, 0 };
	}
	else if (GetAsyncKeyState(VK_NUMPAD5) & 0x1)
	{
		spotLight.ConeDir = { 0, -1, 0 };
	}
	else if (GetAsyncKeyState(VK_NUMPAD8) & 0x1)
	{
		spotLight.ConeDir = { 0, -1, 1 };
	}
	else if (GetAsyncKeyState(VK_NUMPAD2) & 0x1)
	{
		spotLight.ConeDir = { 0, -1, -1 };
	}

	if (spotLight.position.x < -5 || spotLight.position.x > 5)
	{
		SLVelocity.x = -SLVelocity.x;
	}
	if (spotLight.position.z < -5 || spotLight.position.z > 5)
	{
		SLVelocity.y = -SLVelocity.y;
	}

	spotLight.position.x += SLVelocity.x * (float)timer;
	spotLight.position.z += SLVelocity.y * (float)timer;

	
}
void DEMO_APP::DrawSecondView()
{
	D3D11_MAPPED_SUBRESOURCE MapSubresource;
	unsigned int VertexSize = sizeof(SIMPLE_VERTEX);
	unsigned int Array[] = { 0 };
	float Colour[4] = { 200.0f / 255.0f, 0, 220.0f / 225.0f, 0 / 255.0f };

	deferredCont->RSSetViewports(1, SecondViewport);
	deferredCont->OMSetRenderTargets(1, &SecondViewRTV.p, SecondViewDPV);

	memset(&MapSubresource, 0, sizeof(MapSubresource));
	deferredCont->Map(ViewportConstBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
	memcpy(MapSubresource.pData, &ViewPlainWorld, sizeof(ViewPlainWorld));
	deferredCont->Unmap(ViewportConstBuff, 0);

	memset(&MapSubresource, 0, sizeof(MapSubresource));
	deferredCont->Map(ConstantBufferToSendScene, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapSubresource);
	memcpy(MapSubresource.pData, &SecondViewMatrices, sizeof(SecondViewMatrices));
	deferredCont->Unmap(ConstantBufferToSendScene, 0);


	deferredCont->VSSetConstantBuffers(0, 1, &ViewportConstBuff.p);
	deferredCont->VSSetConstantBuffers(1, 1, &ConstantBufferToSendScene.p);
	deferredCont->PSSetShaderResources(0, 1, &SecondViewSRV.p);
	deferredCont->PSSetSamplers(0, 1, &pFloorSamplerState.p);
	deferredCont->IASetVertexBuffers(0, 1, &ViewportPlainVBuff.p, &VertexSize, Array);
	deferredCont->IASetIndexBuffer(ViewportPlainIBuff.p, DXGI_FORMAT_R32_UINT, 0);
	deferredCont->PSSetShader(pLightingPS.p, NULL, 0);
	deferredCont->VSSetShader(pObjectVS.p, NULL, 0);
	deferredCont->IASetInputLayout(InputLayout.p);
	deferredCont->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deferredCont->DrawIndexed(6, 0, 0);
}




