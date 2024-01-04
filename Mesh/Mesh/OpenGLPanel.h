#pragma once
#include "Common.h"

namespace Mesh {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// OpenGLPanel 的摘要
	/// </summary>
	public ref class OpenGLPanel : public System::Windows::Forms::UserControl
	{

	public: void makeContextCurrent();
	public: HGLRC getGlContext();
	public: bool createContext;
	public: HGLRC existingContext;
	private: HWND hWnd;
	private: HDC  hDC;
	private: HGLRC hRC;
	private: bool glInitialized;
	private: void initOpenGL();
	protected: System::Void OnLoad(System::EventArgs^ e) override;
	protected: System::Void OnPaint(System::Windows::Forms::PaintEventArgs^ e) override;
	protected: System::Void OnResize(System::EventArgs^ e) override;

	public:
		OpenGLPanel(void)
		{
			InitializeComponent();
			//
			//TODO:  在此加入建構函式程式碼
			//
			glInitialized = false;
			this->SetStyle(ControlStyles::Opaque, true);
		}

	protected:
		/// <summary>
		/// 清除任何使用中的資源。
		/// </summary>
		~OpenGLPanel()
		{
			if (components)
			{
				delete components;
			}
		}

	private:
		/// <summary>
		/// 設計工具所需的變數。
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// 此為設計工具支援所需的方法 - 請勿使用程式碼編輯器修改
		/// 這個方法的內容。
		/// </summary>
		void InitializeComponent(void)
		{
			this->SuspendLayout();
			// 
			// OpenGLPanel
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Name = L"OpenGLPanel";
			this->ResumeLayout(false);

		}
#pragma endregion
	};
}
