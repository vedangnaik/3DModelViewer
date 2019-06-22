#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ShaderProgram.h>
#include <Camera.h>
#include <Model.h>
#include <Texture2D.h>


unsigned int SCR_WIDTH = 1920;
unsigned int SCR_HEIGHT = 1080;
float ASP_RATIO = 1.777f;


int main() {
	// Initialize GLFW and GLAD
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Loading Model...", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwMaximizeWindow(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	// Initialize Dear ImGUI with the provided OpenGL bindings.
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();


	// Load main shader program
	glEnable(GL_DEPTH_TEST);
	ShaderProgram mainSP = ShaderProgram("shaders/main.vs", "shaders/main.fs");
	Model model = Model("spaceship.dae");
	Camera cam = Camera(
		glm::vec3(0.0f, 0.0f, 10.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		window
	);
	
	// A texture to hold the rendered scene
	unsigned int tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	// Custom framebuffer to render the scene onto the above texture
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// Main render loop
	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();		
		ImGui::NewFrame();


		// Rendering options pane..
		//	Set startup window size and position, and restrict resize to horizontal only
		ImGui::SetNextWindowSize(ImVec2(500, SCR_HEIGHT), ImGuiCond_Once);
		ImGui::SetNextWindowSizeConstraints(ImVec2(0, -1), ImVec2(FLT_MAX, -1));
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
		//	Constrain the window to no movement
		ImGui::Begin("Options", NULL, ImGuiWindowFlags_NoMove);
		//	Save the current width of the window for the model pane to use
		int optionsPaneWidth = ImGui::GetWindowSize().x;
		ImGui::End();
		

		// Bind custom framebuffer so rendered scene is stored in the texture
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glBindTexture(GL_TEXTURE_2D, tex);
		// Clear the buffer before use
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Render the scene
		mainSP.use();
		mainSP.setUniformMat4("model", glm::mat4(1.0f));
		mainSP.setUniformMat4("view", cam.getViewMat());
		mainSP.setUniformMat4("projection", glm::perspective(glm::radians(45.0f), 1.777f, 0.1f, 100.0f));
		model.draw();
		// Go back to default framebuffer so that ImGUI windows can be seen
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		model.draw();

		// Rendering model pane..
		//	Same as above
		ImGui::SetNextWindowSize(ImVec2(SCR_WIDTH - 500, SCR_HEIGHT), ImGuiCond_Once);
		ImGui::SetNextWindowSizeConstraints(ImVec2(0, -1), ImVec2(FLT_MAX, -1));
		ImGui::SetNextWindowPos(ImVec2(500, 0), ImGuiCond_Once);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 1));
		ImGui::Begin("Model", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
		//	Move the pane to the right by the width of the options pane..
		ImGui::SetWindowPos(ImVec2(optionsPaneWidth, 0));
		//	.. then horizontally shrink it by the same amount. The window dimensions are needed later, hence the variable
		ImVec2 windowDims = ImVec2(SCR_WIDTH - optionsPaneWidth, SCR_HEIGHT);
		ImGui::SetWindowSize(windowDims);
		//	We want to maintain the scene's aspect ratio when the model pane is resized, so the scale factor is calculated here
		float scale = (float)(SCR_WIDTH - optionsPaneWidth) / (float)SCR_WIDTH;
		//	The image dimensions are thus
		ImVec2 imageDims = ImVec2(SCR_WIDTH - optionsPaneWidth, SCR_HEIGHT * scale);
		//	Set the window cursor in the middle so that the image stays centered
		ImGui::SetCursorPos(ImVec2(
			(windowDims.x - imageDims.x) * 0.4f, // Weird factor to compensate for taskbar
			(windowDims.y - imageDims.y) * 0.4f
		));
		//	Attach the image to the window
		ImGui::Image(
			(void*)tex, 
			ImVec2(imageDims), 
			ImVec2(0, 1), // The image is flipped, so these
			ImVec2(1, 0)  // UVs are flipped to compensate
		);
		ImGui::End();
		ImGui::PopStyleColor();

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