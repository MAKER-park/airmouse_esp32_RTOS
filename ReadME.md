<!-- 
Widget https://github.com/anuraghazra/github-readme-stats
Badges https://github.com/Ileriayo/markdown-badges
Icons  https://gist.github.com/rxaviers/7360908
 -->

<h1 align="center">PARK JAE WAN _ ESP 32 AIR MOUSE BLE🐾</h1>

<!-- LANGUAGES Widget -->
<!-- ![Top Langs](https://github-readme-stats.vercel.app/api/top-langs/?username=nrmhvr&exclude_repo=github-readme-stats&hide=ANTLR,Jasmin&langs_count=6&layout=compact&hide_border=true&theme=gruvbox_light) -->
<!--
<img align="left" src="https://github-readme-stats.vercel.app/api?username=nrmhvr&theme=gruvbox_light&hide_border=true&count_private=true&show_icons=false&custom_title=GitHub%20Stats😊"/>
-->

<h2>💻 Working with</h2>
<img src="https://img.shields.io/badge/C++-cppplus-blue?logo=cplusplus&logoColor=white">
<img src="https://img.shields.io/badge/C-C-blue?logo=c&logoColor=white">
<img src="https://img.shields.io/badge/espressif-espressif-blue?logo=espressif&logoColor=#E7352C">

<!--<h2>📚 Tech Stack</h2> -->
<!-- Languages 
![Java](https://img.shields.io/badge/java-%23ED8B00.svg?style=for-the-badge&logo=java&logoColor=white) ![Python](https://img.shields.io/badge/python-3670A0?style=for-the-badge&logo=python&logoColor=ffdd54) ![CSS3](https://img.shields.io/badge/css3-%231572B6.svg?style=for-the-badge&logo=css3&logoColor=white) ![C](https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white)
-->

<!-- Server
![Apache](https://img.shields.io/badge/apache-%23D42029.svg?style=for-the-badge&logo=apache&logoColor=white)
-->
<!-- DB 
![MySQL](https://img.shields.io/badge/mysql-%2300f.svg?style=for-the-badge&logo=mysql&logoColor=white) 
-->
<!-- Frameworks, Platforms and Libraries -->
<!-- ![Vue.js](https://img.shields.io/badge/vuejs-%2335495e.svg?style=for-the-badge&logo=vuedotjs&logoColor=%234FC08D) ![Anaconda](https://img.shields.io/badge/Anaconda-%2344A833.svg?style=for-the-badge&logo=anaconda&logoColor=white) ![Flask](https://img.shields.io/badge/flask-%23000.svg?style=for-the-badge&logo=flask&logoColor=white)  -->

<!-- IDE & Editors -->
<!-- ![Visual Studio Code](https://img.shields.io/badge/Visual%20Studio%20Code-0078d7.svg?style=for-the-badge&logo=visual-studio-code&logoColor=white) ![Atom](https://img.shields.io/badge/Atom-%2366595C.svg?style=for-the-badge&logo=atom&logoColor=white) ![Eclipse](https://img.shields.io/badge/Eclipse-FE7A16.svg?style=for-the-badge&logo=Eclipse&logoColor=white) ![Jupyter Notebook](https://img.shields.io/badge/jupyter-%23FA0F00.svg?style=for-the-badge&logo=jupyter&logoColor=white) -->

<!-- <h2>✏️ learning</h2> -->
<!-- <h2>💡 📁 Projects</h2> -->

## 기능

- ESP32 보드를 사용하여 무선 마우스 제어
- 가속도 센서를 통한 움직임 감지
- 클릭 및 스크롤 제스처 인식
- 블루투스 연결을 통한 대상 기기의 마우스 포인터 위치 변경 및 클릭 HID 제어

## 사용 방법

아래의 단계를 따라 AirMouse를 설정하고 사용할 수 있습니다:

1. 필요한 하드웨어를 구성합니다:
   - ESP32 개발 보드
   - 가속도 센서 모듈(MPU6050)
   - USB 케이블
   - PUSH BUTTON * 2 EA
   - 여기 부터는 옵션 입니다.(센서 모니터링 및 디버깅 용도) 
   - OLED 디스플레이
   - 5 WAY Tactile Switch
<div> 
연결방법은 추후 추가될 예정이며 여기서 확인 가능할겁니다.
</div> 

2. ESP32 보드에 해당 소스 코드를 업로드합니다. 자세한 업로드 방법은 `추후 블로그에 기재될 예정`를 참조하세요.

3. AirMouse를 사용하려는 컴퓨터 또는 안드로이드 기기에 블루투스를 이용해서 연결 합니다.

4. 컴퓨터에서 USB 시리얼 터미널 프로그램을 열고, ESP32의 시리얼 포트와 속도를 설정합니다.<디버깅 확인용도>

5. ESP32 보드를 컴퓨터에 연결한 상태에서 마우스로 움직여 보세요. 움직임이 컴퓨터또는 안드로이드 모바일 기기 상에서 포인터 위치에 반영되어야 합니다.

6. 동작에 대한 결과는 

<iframe width="560" height="315" src="https://www.youtube.com/embed/oS-1Ow5L00g" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" allowfullscreen></iframe>

 다음 영상과 비교할 수 있습니다.


## 기여하기

이 프로젝트에 기여하고 싶다면, 다음 단계를 따라주세요:

1. 이 리포지토리를 포크합니다.

2. 개선하거나 수정하고자 하는 기능을 위한 새로운 브랜치를 생성합니다.

3. 변경 사항을 커밋하고, 새로운 브랜치에 푸시합니다.

4. Pull Request를 작성하여 변경 사항을 원본 리포지토리로 제출합니다.

5. 리뷰 및 피드백을 통해 변경 사항을 검토합니다.

## 라이선스

이 프로젝트는 MIT 라이선스에 따라 배포됩니다. 자세한 내용은 `LICENSE` 파일을 참조하세요.