
#include "VEInclude.h"
#include "GUIListener.h"
#include <GLFW/glfw3.h>





namespace ve
{

	void GUIListener::handleKey(veEvent event, bool activatedEvent, nk_text_edit edit) {
		if (activatedEvent) {
			if (event.idata3 == GLFW_PRESS) {
				if (event.idata1 == GLFW_KEY_1) {
					nk_str_insert_str_char(&edit.string, edit.cursor, "1");
				}
				else if (event.idata1 == GLFW_KEY_2) {
					nk_str_insert_str_char(&edit.string, edit.cursor, "2");
				}
				else if (event.idata1 == GLFW_KEY_3) {
					nk_str_insert_str_char(&edit.string, edit.cursor, "3");
				}
				else if (event.idata1 == GLFW_KEY_4) {
					nk_str_insert_str_char(&edit.string, edit.cursor, "4");
				}
				else if (event.idata1 == GLFW_KEY_5) {
					nk_str_insert_str_char(&edit.string, edit.cursor, "5");
				}
				else if (event.idata1 == GLFW_KEY_6) {
					nk_str_insert_str_char(&edit.string, edit.cursor, "6");
				}
				else if (event.idata1 == GLFW_KEY_7) {
					nk_str_insert_str_char(&edit.string, edit.cursor, "7");
				}
				else if (event.idata1 == GLFW_KEY_8) {
					nk_str_insert_str_char(&edit.string, edit.cursor, "8");
				}
				else if (event.idata1 == GLFW_KEY_9) {
					nk_str_insert_str_char(&edit.string, edit.cursor, "9");
				}
				else if (event.idata1 == GLFW_KEY_0) {
					nk_str_insert_str_char(&edit.string, edit.cursor, "0");
				}
				else if (event.idata1 == GLFW_KEY_PERIOD) {
					nk_str_insert_str_char(&edit.string, edit.cursor, ".");
				}
				else if (event.idata1 == GLFW_KEY_BACKSPACE) {
					nk_str_delete_chars(&edit.string, edit.cursor, 1);
					
				}
			}

		}

	}


	bool GUIListener::onKeyboard(veEvent event) {

		
		
		if (event.idata1 == GLFW_KEY_I && event.idata3 == GLFW_PRESS) {

			if (!g_menu) {
				g_menu = true;
				
			}
			else {
				g_menu = false;
				
			}
		}


		handleKey(event, editactivatedX, xedit);
		handleKey(event, editactivatedY, yedit);
		handleKey(event, editactivatedZ, zedit);
		
		
		return false;
		
		

	}

	nk_color convertColorFromVec4(glm::vec4 colorv) {

		nk_color nuk_c;

		nuk_c.r = (nk_byte)colorv.r;
		nuk_c.g = (nk_byte)colorv.g;
		nuk_c.b = (nk_byte)colorv.b;
		nuk_c.a = (nk_byte)colorv.a;



		return nuk_c;


	}

	glm::vec4 convertColorFromNK(nk_color nuk_c) {
		glm::vec4 colorVec;

		colorVec.r = std::max((float)nuk_c.r/255.f, 0.0f);
		colorVec.g = std::max((float)nuk_c.g / 255.f, 0.0f);
		colorVec.b = std::max((float)nuk_c.b / 255.f, 0.0f);
		colorVec.a = std::max((float)nuk_c.a / 255.f, 0.0f);



		return colorVec;
	}

	glm::vec3 convertToVec3(const char* x, const char* y, const char* z) {
		float xValue = std::stof(x);
		float yValue = std::stof(y);
		float zValue = std::stof(z);


		return glm::vec3(xValue, yValue, zValue);
	}


	void GUIListener::setValues() {
		selectedNode->setPosition(convertToVec3(nk_str_get(&xedit.string), nk_str_get(&yedit.string), nk_str_get(&zedit.string)));

	}



	 void GUIListener::onDrawOverlay(veEvent event) {

		



		 VESubrender_Nuklear* pSubrender = (VESubrender_Nuklear*)getEnginePointer()->getRenderer()->getOverlay();
		 if (pSubrender == nullptr) return;

		 struct nk_context* ctx = pSubrender->getContext();

		 auto win = ((VEWindowGLFW*)getWindowPointer())->getWindowHandle();

		 if (scenenodes.empty()) {

			 VESceneNode* node = getSceneManagerPointer()->getRootSceneNode();
			 std::queue<VESceneNode*> sceneNodesQ;
			 sceneNodesQ.push(node);
			 bool buttonpressed = false;

			 lights = getSceneManagerPointer()->getLights();

			 while (!sceneNodesQ.empty()) {
				 for (auto c : sceneNodesQ.front()->getChildrenList()) {
					 std::cout << c->getName() << std::endl;
					 if (c->getNodeType()) {
						 scenenodes.push_back(c);
					 }
					 sceneNodesQ.push(c);

				 }

				 sceneNodesQ.pop();
			 }
		 }




		 if (g_menu) {
			
		 

			if(windowRect.x == 0){

				 auto rect = nk_rect(0, 200, 500, 500);
				 windowRect = rect;
				

				 nk_begin(ctx, windowName.c_str(), windowRect, windowOptions);
				 nk_layout_row_dynamic(ctx, 25, 1);


				 for (auto c : scenenodes) {
					 if (nk_button_label(ctx, c->getName().c_str())) {

						 std::cout << c->getName() << std::endl;
						 buttonpressed = true;
						 selectedNode = c;
					 }
				 }


				 nk_end(ctx);
			 } else if(!nk_window_is_closed(ctx, windowName.c_str())) {
				 if (nk_begin(ctx, windowName.c_str(), windowRect, windowOptions)) {

					 std::cout << "enter" << std::endl;
					 nk_layout_row_dynamic(ctx, 25, 1);


					 for (auto c : scenenodes) {
						 if (nk_button_label(ctx, c->getName().c_str())) {

							 std::cout << c->getName() << std::endl;
							 buttonpressed = true;
							 selectedNode = c;
						 }
					 }
				
				 }
				 nk_end(ctx);
			 }


			 if (buttonpressed) {


				 if (((VESceneObject*) selectedNode)->getObjectType() == VESceneObject::VE_OBJECT_TYPE_LIGHT) {
					 
					 for (auto light : lights) {

						 if (selectedNode->getName() == light->getName() && selectedNode->getName()=="StandardAmbientLight") {
			
								
							 if (nk_begin(ctx, "ambL", nk_rect(200, 200, 300, 300), NK_WINDOW_BORDER)) {
								 nk_layout_row_dynamic(ctx, 20, 1);
								 
								 
								 ambColor = convertColorFromVec4(light->m_col_ambient);
								
								 

								 nk_layout_row_static(ctx, 250, 250,1);
								 pickedAmbColor = nk_color_picker(ctx, ambColor, NK_RGBA);
								 nk_label(ctx, light->getName().c_str(), NK_TEXT_LEFT);

								 convertedAmbColor = convertColorFromNK(pickedAmbColor);
								
								 if (convertedAmbColor.r > 0) {
									 light->m_col_ambient = convertedAmbColor;
								 }

							 nk_end(ctx);
							 }


						 }

						 if (selectedNode->getName() == light->getName() && selectedNode->getName() == "StandardDirLight") {
							 if (nk_begin(ctx, "dirL", nk_rect(200, 200, 300, 300), NK_WINDOW_BORDER)) {
								 nk_layout_row_dynamic(ctx, 35, 1);

								 color = convertColorFromVec4(light->m_col_ambient);
							
								 nk_layout_row_static(ctx, 150, 250,1);
								  pickedColor = nk_color_picker(ctx, color, NK_RGBA);
								 nk_label(ctx, light->getName().c_str(), NK_TEXT_LEFT);

								
								 convertedColor = convertColorFromNK(pickedColor);

								 if (convertedColor.r > 0) {
									 light->m_col_diffuse = convertedColor;
									 light->m_col_specular = convertedColor;
								 }


								 nk_end(ctx);
							 }


						 }

					 }
					



				 }
				 else {
					 
					
					 

					 if (nk_begin(ctx, "pos", nk_rect(200, 200, 300, 300), NK_WINDOW_BORDER|NK_WINDOW_MOVABLE)) {
						 nk_layout_row_dynamic(ctx, 40, 1);

						 if (currentNode != selectedNode) {
							 nk_str_clear(&xedit.string);
							 nk_str_clear(&yedit.string);
							 nk_str_clear(&zedit.string);

							 nk_str_append_str_char(&xedit.string, std::to_string(selectedNode->getPosition().x).c_str());
							 nk_str_append_str_char(&yedit.string, std::to_string(selectedNode->getPosition().y).c_str());
							 nk_str_append_str_char(&zedit.string, std::to_string(selectedNode->getPosition().z).c_str());
							 currentNode = selectedNode;
						 }

						 auto x = nk_edit_buffer(ctx, NK_EDIT_FIELD, &xedit, nk_filter_float);
						 auto y = nk_edit_buffer(ctx, NK_EDIT_FIELD, &yedit, nk_filter_float);
						 auto z = nk_edit_buffer(ctx, NK_EDIT_FIELD, &zedit, nk_filter_float);

						
						 
						 if (xedit.active) {
							 editactivatedX = true;
						 }
						 if (!xedit.active) {
							 editactivatedX = false;
							 setValues();
						 }
						 if (yedit.active) {
							 editactivatedY = true;
						 }
						 if (!yedit.active) {
							 editactivatedY = false;
							 setValues();
						 }
						 if (zedit.active) {
							 editactivatedZ = true;
						 }
						 if (!zedit.active) {
							 editactivatedZ = false;
							 setValues();

						 }
						 
						 
						
						
						 nk_end(ctx);

					 }
				 }

			 }
			

		 };

		 

		
	 }
	

	}


