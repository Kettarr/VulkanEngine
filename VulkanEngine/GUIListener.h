#pragma once


namespace ve
{

	class GUIListener : public VEEventListener
	{

		std::vector<VESceneNode*> scenenodes;
		bool g_menu = false;
		bool buttonpressed = false;
		VESceneNode* selectedNode;
		VESceneNode* currentNode;

		bool valueSet = false;

		nk_color color;
		nk_color ambColor;

		nk_color pickedColor;
		nk_color pickedAmbColor;

		glm::vec4 convertedAmbColor;
		glm::vec4 convertedColor;
		

		bool initialcolor = false;
		bool initialambcolor = false;


		struct nk_rect windowPosRect = nk_rect(0, 0, 0, 0);
		struct nk_rect windowRect = nk_rect(0, 0, 0, 0);
		std::string windowName = "Nodes";
		int windowOptions = NK_WINDOW_BORDER;
		int windowPosOptions = NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_CLOSABLE;

		nk_text_edit xedit;
		nk_text_edit yedit;
		nk_text_edit zedit;


		bool editactivatedX = false;
		bool editactivatedY = false;
		bool editactivatedZ = false;

		std::vector<VELight*> lights;
		

		virtual bool onKeyboard(veEvent event);
		virtual void onDrawOverlay(veEvent event);

		virtual void handleKey(veEvent event, bool activatedEvent, nk_text_edit edit);
		virtual void setValues();
	public:
		GUIListener(std::string name) : VEEventListener(name) {
			nk_textedit_init_default(&xedit);
			nk_textedit_init_default(&yedit);
			nk_textedit_init_default(&zedit);
		};

		virtual ~GUIListener() {}
		
	};

}