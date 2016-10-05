# Файл содержит переменные с путями поиска заголовков и библиотек
# для проектов из состава cg_course_examples.

# На платформе Windows вам потребуется:
#   - настроить переменную окружения SDK2015 на своём компьютере, чтобы она указывала на корневой каталог с библиотеками
#   - в корневом каталоге с библиотеками скачать и собрать необходимые библиотеки (SDL2, boost и т.д.), чтобы предоставленные пути совпадали

if(WIN32)
	if (CMAKE_BUILD_TYPE EQUAL "Debug")
		set(CG_DEBUG_SUFFIX "d")
	else (CMAKE_BUILD_TYPE EQUAL "Debug")
		set(CG_DEBUG_SUFFIX "")
	endif (CMAKE_BUILD_TYPE EQUAL "Debug")

	# Пути поиска заголовочных файлов
	set(CG_WIN32_INCLUDE_PATHS
		 "$ENV{SDK2015}/boost/include"
		 "$ENV{SDK2015}/SDL2/include"
		 "$ENV{SDK2015}/glm"
		 "$ENV{SDK2015}/glew/include")
 
 # Пути поиска библиотек
 # Переменные вида $(VAR) остаются нерасрытыми в CMake,
 # Их раскроет Visual Studio
 
 # Для 32-битных платформ
	if( CMAKE_SIZEOF_VOID_P EQUAL 4)
	
		set(CG_WIN32_LINKER_PATHS
			 "$ENV{SDK2015}/boost/lib"
			 "$ENV{SDK2015}/SDL2/lib/x86"
			 "$ENV{SDK2015}/glew/lib/$(Configuration)/Win32")
			 
 # Для 64-битных платформ
	else( CMAKE_SIZEOF_VOID_P EQUAL 4)
	
	endif( CMAKE_SIZEOF_VOID_P EQUAL 4)
 
endif(WIN32)
