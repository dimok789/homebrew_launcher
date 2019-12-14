# WiiU 홈브류 런처

홈브류 런처는 SD 카드에 있는 홈브류 응용 프로그램을 나열하고 (Wii의 홈브류 채널과 유사한) 응용 프로그램을 실행할 수 있는 WiiU 홈브류입니다.

#### 사용법

홈브류 런처 (또는 간단히 HBL)를 사용하려면 homebrew_launcher.elf를 SD:/wiiu/apps/homebrew_launcher/homebrew_launcher.elf에 복사하고 WiiU 브라우저를 통해 설치 프로그램을 실행해야 합니다.

나열될 앱은 SD 카드의 루트에서 /wiiu/apps/homebrew_name/some_elf_name.elf 경로에 있어야 합니다. meta.xml 및 icon.png (256x96)는 선택 사항입니다. 예를 들면 다음과 같습니다.

- sd:/
  - wiiu/
    - apps/
     - homebrew_launcher/
        - homebrew_launcher.elf
        - meta.xml
        - icon.png
     - loadiine_gx2/
       - loadiine_gx2.elf
       - meta.xml
       - icon.png
     - ddd/
       - ddd.elf
       - meta.xml
       - icon.png
     - ftpiiu/
       - ftpiiu.elf
       - meta.xml
       - icon.png

#### 홈브류 런처 빌드
기본 응용 프로그램을 빌드하려면 devkitPPC와 일부 추가 라이브러리가 필요합니다. 아직 완료되지 않은 경우 devkitPPC 및 devkitPro의 경로를 환경 변수 DEVKITPRO 및 DEVKITPPC로 내보내십시오.

일부는 이 저장소의 "libs" 폴더에 있습니다. "kitlibs.zip"을 devkitPro 디렉토리에 추출하십시오. 이 패키지는 다음을 포함합니다:
- zlib

이 응용 프로그램을 빌드하려면 여러개의 라이브러리가 필요합니다:

- 기능 접속을 위한 [dynamic_libs](https://github.com/Maschell/dynamic_libs/tree/lib)
- 공통 기능을 위한 [libutils](https://github.com/Maschell/libutils)
- GUI 요소를 위한 [libgui](https://github.com/Maschell/libgui)

README에 따라 (이 순서대로) 설치하십시오. 라이브러리 자체의 종속성을 잊지 마십시오.

나머지는 기본 응용 프로그램 경로를 입력하고 "make"를 입력하는 것입니다. 기본 경로에 homebrew_launcher.elf 및 homebrew__launcher_dbg.elf가 있어야 합니다.

설치 프로그램을 컴파일하려면 소스 코드에 "installer" 경로를 입력하고 "make"를 입력하십시오.

#### Homebrew Launcher 용 응용 프로그램 / homebrew (ELF) 빌드 
HBL를 위한 애플리케이션을 빌드하는 방법에 대한 예는 [Hello World 예제](https://github.com/dimok789/hello_world) 애플리케이션 또는 HBL를 위한 libwiiu 애플리케이션의 포트 [Pong](https://github.com/dimok789/pong_port)를 참조하십시오.

#### Meta XML

meta.xml은 선택 사항이며 홈브류 ELF 파일과 동일한 경로에 넣어 홈브류에 대한 추가 정보를 표시할 수 있습니다.

다음은 XML 예제입니다:

    <?xml version="1.0" encoding="UTF-8" standalone="yes"?>
    <app version="1">
      <name>name</name>
      <coder>coder</coder>
      <version>app version</version>
      <release_date>app release date</release_date>
      <short_description>short description</short_description> 
      <long_description>long description</long_description> 
    </app>

#### PNG 아이콘
icon.png의 해상도는 256 x 96이어야하며 홈브류 ELF 파일과 동일한 경로에 배치할 수 있습니다. 이 파일은 선택 사항이며 홈브류 런처 안에 홈브류 아이콘이 표시됩니다.
