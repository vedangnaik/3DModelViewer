#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <portable-file-dialogs.h>
#include <stb_image.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ShaderProgram.h>
#include <Model.h>
#include <Light.h>


// Application constants
//	Screen constants, adjust it to your native resolution.
//	TODO find a way to get this automatically, I have been unable to so far.
unsigned int SCR_WIDTH = 1920;
unsigned int SCR_HEIGHT = 1080;
float ASP_RATIO = 1.777f;
//	Model matrix globally defined as it is directly controlled by the user
glm::mat4 modelMatrix = glm::mat4(1.0f);
//	Control sensitivities
//	TODO set these from the GUI
float rotatingAngle = 5.0f; // degrees, set to preference
float translationalOffset = 0.25f; // unitless, set to preference
float scaleFactor = 0.05f;
//	Texture handles
unsigned int textureHandles[5];
std::string textureNames[] = {
	"albedoMap",
	"normalMap",
	"metallicMap",
	"roughnessMap",
	"aoMap"
};
//	Point light modification and creation window parameters
bool pointLightCreateWinOpen = false;
bool pointLightModWinOpen = false;
PointLight* pointLightToMod;
//	Directional light modification window parameters
bool dirLightModWinOpen = false;


// Callback function that controls movement of the model
// TODO Maybe find a way to do it without glm::inverse()
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// Handle roll around x-axis
	if (key == GLFW_KEY_W && action == GLFW_REPEAT) {
		modelMatrix = glm::rotate(
			modelMatrix,
			glm::radians(rotatingAngle),
			glm::mat3(glm::inverse(modelMatrix)) * glm::vec3(1.0f, 0.0f, 0.0f)
		);
	}
	if (key == GLFW_KEY_S && action == GLFW_REPEAT) {
		modelMatrix = glm::rotate(
			modelMatrix, 
			glm::radians(-rotatingAngle),
			glm::mat3(glm::inverse(modelMatrix)) * glm::vec3(1.0f, 0.0f, 0.0f)
		);
	}
	// Handle roll around y-axis
	if (key == GLFW_KEY_A && action == GLFW_REPEAT) {
		modelMatrix = glm::rotate(
			modelMatrix,
			glm::radians(rotatingAngle),
			glm::mat3(glm::inverse(modelMatrix)) * glm::vec3(0.0f, 1.0f, 0.0f)
		);
	}
	if (key == GLFW_KEY_D && action == GLFW_REPEAT) {
		modelMatrix = glm::rotate(
			modelMatrix,
			glm::radians(-rotatingAngle),
			glm::mat3(glm::inverse(modelMatrix)) * glm::vec3(0.0f, 1.0f, 0.0f)
		);
	}
	// Handle roll around z-axis
	if (key == GLFW_KEY_Q && action == GLFW_REPEAT) {
		modelMatrix = glm::rotate(
			modelMatrix,
			glm::radians(rotatingAngle),
			glm::mat3(glm::inverse(modelMatrix)) * glm::vec3(0.0f, 0.0f, 1.0f)
		);
	}
	if (key == GLFW_KEY_E && action == GLFW_REPEAT) {
		modelMatrix = glm::rotate(
			modelMatrix,
			glm::radians(-rotatingAngle),
			glm::mat3(glm::inverse(modelMatrix)) * glm::vec3(0.0f, 0.0f, 1.0f)
		);
	}

	// Handle translational movement about x-axis
	if (key == GLFW_KEY_RIGHT && action == GLFW_REPEAT) {
		modelMatrix = glm::translate(
			modelMatrix,
			translationalOffset * (glm::mat3(glm::inverse(modelMatrix)) * glm::vec3(1.0f, 0.0f, 0.0f))
		);
	}
	if (key == GLFW_KEY_LEFT && action == GLFW_REPEAT) {
		modelMatrix = glm::translate(
			modelMatrix,
			translationalOffset * (glm::mat3(glm::inverse(modelMatrix)) * glm::vec3(-1.0f, 0.0f, 0.0f))
		);
	}
	// Handle translational movement about y-axis
	if (key == GLFW_KEY_UP && action == GLFW_REPEAT) {
		modelMatrix = glm::translate(
			modelMatrix,
			translationalOffset * (glm::mat3(glm::inverse(modelMatrix)) * glm::vec3(0.0f, 1.0f, 0.0f))
		);
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_REPEAT) {
		modelMatrix = glm::translate(
			modelMatrix,
			translationalOffset * (glm::mat3(glm::inverse(modelMatrix)) * glm::vec3(0.0f, -1.0f, 0.0f))
		);
	}

	// Handle zooming in and out
	if (key == GLFW_KEY_Z && action == GLFW_REPEAT) {
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f + scaleFactor));
	}
	if (key == GLFW_KEY_X && action == GLFW_REPEAT) {
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f - scaleFactor));
	}
}

// Helper function to create textures from filepaths
unsigned int createTexture(const char* texturePath) {
	unsigned int textureID;
	int width, height, nrChannels;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	unsigned char* data = stbi_load(texturePath, &width, &height, &nrChannels, 0);
	if (data) {
		GLenum format;
		if (nrChannels == 3) format = GL_RGB;
		else if (nrChannels == 4) format = GL_RGBA;
		else if (nrChannels == 1) format = GL_RED;
		else {
			std::cout << "# channels: " << nrChannels << std::endl;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	return textureID;
}


// Main program
int main() {
	// Initialize GLFW and GLAD
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Loading Model...", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwMaximizeWindow(window);
	glfwSetKeyCallback(window, keyCallback);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	// Initialize Dear ImGUI with the provided OpenGL bindings.
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.FontGlobalScale *= 1.8f;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();


	// View and projection matrices
	glm::mat4 viewMatrix = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 10.0f),
		glm::vec3(0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 10.0f);
	glm::mat4 projectionMatrix = glm::perspective(
		glm::radians(45.0f),
		ASP_RATIO,
		0.1f,
		100.0f
	);

	// Lighting
	std::vector<PointLight> pointLights;
	pointLights.push_back(PointLight{
		glm::vec3(-10.0f, 10.0f, 10.0f),
		glm::vec3(100.0f, 100.0f, 100.0f)
	});
	DirectionalLight dirLight = DirectionalLight{
		glm::vec3(-10.0f, -10.0f, 0.0f),
		glm::vec3(100.0f, 50.0f, 31.0f)
	};
	std::vector<SpotLight> spotLights;
	spotLights.push_back(SpotLight{
		glm::vec3(0.0f, 0.0f, 10.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		cos(glm::radians(12.5f)),
		glm::vec3(100.0f, 50.0f, 31.0f)
	});

	// Load default  model and textures
	Model model = Model("assets/crate.3ds");
	textureHandles[0] = createTexture("assets/stone/stone-albedo.png");
	textureHandles[1] = createTexture("assets/stone/stone-normal.png");
	textureHandles[2] = createTexture("assets/stone/stone-metalness.png");
	textureHandles[3] = createTexture("assets/stone/stone-rough.png");
	textureHandles[4] = createTexture("assets/stone/stone-ao.png");


	// Load shaders
	ShaderProgram mainSP = ShaderProgram("shaders/main.vs", "shaders/main.fs");


	// Main render loop
	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


		// Main Menubar declaration
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::Button("Open Model")) {
				pfd::open_file f = pfd::open_file("Select your file: ");
				model = Model(f.result()[0].c_str());
			}
			if (ImGui::BeginMenu("Textures")) {				
				if (ImGui::Button("Choose Albedo Map")) {
					pfd::open_file f = pfd::open_file("Select albedo map");
					textureHandles[0] = createTexture(f.result()[0].c_str());
				}
				if (ImGui::Button("Choose Normal Map")) {
					pfd::open_file f = pfd::open_file("Select normal map");
					textureHandles[1] = createTexture(f.result()[0].c_str());
				}
				if (ImGui::Button("Choose Metallic Map")) {
					pfd::open_file f = pfd::open_file("Select metallic map");
					textureHandles[2] = createTexture(f.result()[0].c_str());
				}
				if (ImGui::Button("Choose Roughness Map")) {
					pfd::open_file f = pfd::open_file("Select roughness map");
					textureHandles[3] = createTexture(f.result()[0].c_str());
				}
				if (ImGui::Button("Choose AO Map")) {
					pfd::open_file f = pfd::open_file("Select AO map");
					textureHandles[4] = createTexture(f.result()[0].c_str());
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Lighting")) {
				if (ImGui::BeginMenu("Point Lights")) {
					if (ImGui::Button("Add Light")) {
						pointLightCreateWinOpen = true;
					}
					ImGui::Separator();
					for (int i = 0; i < pointLights.size(); i++) {
						std::string temp = "Light ";
						temp += std::to_string(i);
						if (ImGui::MenuItem(temp.c_str())) {
							// Sets these global variables so that they can be modified
							pointLightModWinOpen = true;
							pointLightToMod = &pointLights[i];
						}
					}
					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Directional Light")) {
					dirLightModWinOpen = true;
				}
				ImGui::EndMenu();
			}
			if (ImGui::Button("Reset")) {
				modelMatrix = glm::mat4(1.0f);
			}
			ImGui::EndMainMenuBar();
		}
		// Keep the point light modification and creation windows open and modify pointLightToMod
		if (pointLightModWinOpen) {
			ImGui::Begin("Modifying point light", &pointLightModWinOpen, ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::Text("Position");
			ImGui::InputFloat("x", &pointLightToMod->position.x, 0.1f, 1.0f);
			ImGui::InputFloat("y", &pointLightToMod->position.y, 0.1f, 1.0f);
			ImGui::InputFloat("z", &pointLightToMod->position.z, 0.1f, 1.0f);
			ImGui::Separator();
			ImGui::Text("Color");
			ImGui::InputFloat("r", &pointLightToMod->color.r, 0.1f, 1.0f);
			ImGui::InputFloat("g", &pointLightToMod->color.g, 0.1f, 1.0f);
			ImGui::InputFloat("b", &pointLightToMod->color.b, 0.1f, 1.0f);
			ImGui::Separator();
			ImGui::Text("Attenuation");
			ImGui::InputFloat("attConstant", &pointLightToMod->attConstant, 0.1f, 1.0f);
			ImGui::InputFloat("attLinear", &pointLightToMod->attLinear, 0.1f, 1.0f);
			ImGui::InputFloat("attQuadratic", &pointLightToMod->attQuadratic, 0.1f, 1.0f);
			ImGui::Separator();
			if (ImGui::Button("Delete")) {
				for (int i = 0; i < pointLights.size(); i++) {
					if (pointLights[i] == *pointLightToMod) {
						pointLights.erase(pointLights.begin() + i);
						break;
					}
				}
			}
			ImGui::End();
		}
		if (pointLightCreateWinOpen) {
			PointLight toAdd;
			ImGui::Begin("Adding Light", &pointLightCreateWinOpen, ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::Text("Position");
			ImGui::InputFloat("x", &toAdd.position.x, 0.1f, 1.0f);
			ImGui::InputFloat("y", &toAdd.position.y, 0.1f, 1.0f);
			ImGui::InputFloat("z", &toAdd.position.z, 0.1f, 1.0f);
			ImGui::Separator();
			ImGui::Text("Color");
			ImGui::InputFloat("r", &toAdd.color.r, 0.1f, 1.0f);
			ImGui::InputFloat("g", &toAdd.color.g, 0.1f, 1.0f);
			ImGui::InputFloat("b", &toAdd.color.b, 0.1f, 1.0f);
			ImGui::Separator();
			ImGui::Text("Attenuation");
			ImGui::InputFloat("attConstant", &toAdd.attConstant, 0.1f, 1.0f);
			ImGui::InputFloat("attLinear", &toAdd.attLinear, 0.1f, 1.0f);
			ImGui::InputFloat("attQuadratic", &toAdd.attQuadratic, 0.1f, 1.0f);
			ImGui::Separator();
			if (ImGui::Button("Add")) {
				pointLights.push_back(toAdd);
			}
			ImGui::End();
		}
		// Keep the directional light modification window open and modify dirLight
		if (dirLightModWinOpen) {
			ImGui::Begin("Modifying directional light", &dirLightModWinOpen, ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::Text("Position");
			ImGui::InputFloat("x", &dirLight.direction.x, 0.1f, 1.0f);
			ImGui::InputFloat("y", &dirLight.direction.y, 0.1f, 1.0f);
			ImGui::InputFloat("z", &dirLight.direction.z, 0.1f, 1.0f);
			ImGui::Separator();
			ImGui::Text("Color");
			ImGui::InputFloat("r", &dirLight.color.r, 0.1f, 1.0f);
			ImGui::InputFloat("g", &dirLight.color.g, 0.1f, 1.0f);
			ImGui::InputFloat("b", &dirLight.color.b, 0.1f, 1.0f);
			ImGui::End();
		}


		// Set vertex shader uniforms
		mainSP.use();
		mainSP.setUniformMat4("model", modelMatrix);
		mainSP.setUniformMat3("inverseModel", glm::mat3(glm::transpose(glm::inverse(modelMatrix))));
		mainSP.setUniformMat4("view", viewMatrix);
		mainSP.setUniformMat4("projection", projectionMatrix);
		// Set fragment shader uniforms
		//	Set textures
		for (int i = 0; i < 5; i++) {
			mainSP.setUniformInt(textureNames[i].c_str(), i);
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textureHandles[i]);
		}
		//	Set point lights and number of them
		mainSP.setUniformInt("numPointLights", pointLights.size());
		for (int i = 0; i < pointLights.size(); i++) {
			std::string temp = "pointLights[";
			temp += std::to_string(i) + "]";
			mainSP.setUniformVec3((temp + ".position").c_str(), pointLights[i].position);
			mainSP.setUniformVec3((temp + ".color").c_str(), pointLights[i].color);
			mainSP.setUniformFloat((temp + ".attConstant").c_str(), pointLights[i].attConstant);
			mainSP.setUniformFloat((temp + ".attLinear").c_str(), pointLights[i].attLinear);
			mainSP.setUniformFloat((temp + ".attQuadratic").c_str(), pointLights[i].attQuadratic);
		}
		//	Set directional light
		mainSP.setUniformVec3("dirLight.direction", dirLight.direction);
		mainSP.setUniformVec3("dirLight.color", dirLight.color);
		//	Set spotlights and number of them
		mainSP.setUniformInt("numSpotLights", spotLights.size());
		for (int i = 0; i < spotLights.size(); i++) {
			std::string temp = "spotLights[";
			temp += std::to_string(i) + "]";
			mainSP.setUniformVec3((temp + ".position").c_str(), spotLights[i].position);
			mainSP.setUniformVec3((temp + ".direction").c_str(), spotLights[i].direction);
			mainSP.setUniformFloat((temp + ".cosineCutoff").c_str(), spotLights[i].cosineCutoff);
			mainSP.setUniformVec3((temp + ".color").c_str(), spotLights[i].color);
		}
		//	Set camera position
		mainSP.setUniformVec3("cameraPosition", cameraPosition);
		// Draw model
		model.draw();


		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	// Terminate Dear ImGUI and GLFW
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}