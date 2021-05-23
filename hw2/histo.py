import os
from PIL import Image
import matplotlib
matplotlib.use('TkAgg')
from matplotlib import pyplot as plt
import numpy as np
import csv
import math

class histogram:
    def __init__(self, ROOT_DIR):
        super().__init__()

        #이미지 불러오는 부분

        self.image_name = 'lena_bmp_512x512_new.bmp'

        if os.path.isfile(os.path.join(ROOT_DIR, self.image_name)):
            self.lena = Image.open(os.path.join(ROOT_DIR, self.image_name))


        self.initiate(ROOT_DIR)

        self.control()

    def control(self):      #메뉴를 통해서 각 이미지 처리별 히스토그램 출력 메뉴
        print('==영상처리 HW2 메뉴==')
        print('1. 히스토그램')
        print('2. 평활화 히스토그램')
        print('3. 스트레칭 히스토그램')
        print('4. 엔드인 스트레칭 히스토그램')
        print('0. 종료')


        mode = int(input("번호 입력 :"))


        if mode == 1:
            self.his_1()
        elif mode == 2:
            self.his_2()
        elif mode == 3:
            self.his_3()
        elif mode == 4:
            self.his_4()
        elif mode == 0:
            print('종료합니다.')
            exit()
        else:
            print('0~5값만 입력해주 세요')

    def his_1(self): # 기본 히스토그램   bright_hist x축   pixel_num y축으로 하여서 히스토그램 출력
        plt.plot(self.bright_hist, self.pixel_num)
        plt.title('LENA histogram')
        plt.xlabel('Bright')  #축 라벨
        plt.ylabel('Frequency') #y축 라벨
        plt.show()
        plt.imshow(self.array_lena, cmap='gray')    #그레이 스케일로 출력
        plt.show()

        self.control()


    def his_2(self): # 평활화 히스토그램

        A_sum = [0]*256

        accumulate = 0
        for i, e in enumerate(self.pixel_num):
            accumulate = accumulate + e
            A_sum[i] = A_sum[i] + accumulate

        N_sum = [0]*256 # 정규화 누적 합

        for a in range(256):
            N_sum[a] = 255*A_sum[a]/(512*512) if 255 >= 255*A_sum[a]/262144 else 255 # 255 * 누적합 / 전체 픽셀 수 , 255 넘는 값은 클램핑 -> 정규화 누적합 구함

        eq_intensity = []
        eq_intensity_print = np.array([[0]*512]*512) #이미지 출력위해

        for i, e in enumerate(self.intensity): # 평활화 진행
            eq_intensity.append(int(N_sum[e])) # 값 치환
            x = math.floor(i/512)
            y = int(i%512)
            eq_intensity_print[x][y] = eq_intensity[-1]

        eq_pixel_num = []
        for i in range(256):
            eq_pixel_num.append(eq_intensity.count(i)) # 밝기 값 적용

        plt.plot(self.bright_hist, eq_pixel_num) #히스토 그램 출력
        plt.title('Equalized Histogram')
        plt.xlabel('Bright')
        plt.ylabel('Frequency')
        plt.show()
        plt.title('Equalized image')
        plt.imshow(eq_intensity_print, cmap='gray')
        plt.show()

        self.control()

    def his_3(self):  # 스트레칭 히스토그램     양쪽 끝으로 펼치기
        st_intensity = []
        st_intensity_print = np.array([[0] * 512] * 512)
        st_pixel_num = []
        low = 0
        high = 0

        for i, e in enumerate(self.pixel_num):  # low값 찾기
            if int(e) != 0:
                low = int(i)
                break
        for i in range(low, len(self.pixel_num)):  # high값 찾기
            if int(self.pixel_num[i]) == 0:
                high = int(i)
                break

        for i, e in enumerate(self.intensity):  # 스트레칭 공식이용해서 늘리기
            st_intensity.append(int(255 * ((e - low) / (high - low))))
            x = math.floor(i / 512)
            y = int(i % 512)
            st_intensity_print[x][y] = st_intensity[-1]

        for i in range(256):  # 명암 갯수갑 카운트
            st_pixel_num.append(st_intensity.count(i))

        plt.plot(self.bright_hist, st_pixel_num)#출력부분
        plt.title('Stretched Histogram')
        plt.xlabel('Bright')
        plt.ylabel('Frequency')
        plt.show()
        plt.title('Stretched image')
        plt.imshow(st_intensity_print, cmap='gray')
        plt.show()

        self.control()

    def his_4(self): # 엔드인 탐색 히스토그램

        new_pixel = []
        ei_pixel_num = []
        ei_intensity_print = np.array([[0]*512]*512)

        # 문제에서 주어진 low값 50 / high값 190을 이용해서 엔드인 스트레칭 진행 , 범위가 넘어가면 클램핑 진행
        for i, e in enumerate(self.intensity):
            if e <= 50:
                new_pixel.append(0) #클램핑(0으로)
            elif 50 < e and e < 190:
                new_pixel.append(int((255*(e-50)/140)))  #범위값 안의 데이터는 엔드인 스트레칭 진행
            else:
                new_pixel.append(255) #클램핑(255로)

            x = math.floor(i / 512)
            y = int(i % 512)
            ei_intensity_print[x][y] = new_pixel[-1]

        for i in range(256):
            ei_pixel_num.append(new_pixel.count(i)) # 픽셀 빈도 수 계산하여 넣어주기

        plt.plot(self.bright_hist, ei_pixel_num)
        plt.title('End-In Stretched Histogram')
        plt.xlabel('Bright')
        plt.ylabel('Frequency')
        plt.show()
        plt.title('End-In Stretched image')
        plt.imshow(ei_intensity_print, cmap='gray')
        plt.show()

        self.control()


    def initiate(self, ROOT_DIR):               #불러온 이미지 값 처리
        intensity = []  # 명도값
        bright_hist = []  # 밝기값
        pixel_num = [0 for i in range(256)]  # 0~255밝기값 가진 픽셀갯수만큼 배열에 넣기

        list = []  # csv파일을 읽을 때 받는 list

        array_lena = np.array(self.lena)
        self.array_lena = array_lena

        for i in range(len(array_lena[0])):
            for j in array_lena[i]:
                intensity.append(j)  # 1차원으로 쭉 정렬


        for i in range(256):
            bright_hist.append(i)    # 밝기값에 대한 list값

                                            #csv파일로 저장시켜 한번 해두면 이후에는 더 빨리 연산
        if os.path.exists(
                os.path.join(ROOT_DIR, self.image_name + '.csv')):  #csv파일 검사
            with open(self.image_name + '.csv', 'r') as rfp:  #명암값 읽기
                csv_read = csv.reader(rfp, delimiter=',', quotechar='"')
                for row in csv_read:
                    list.append(row)
            str_pixel_num = list[0]
            pixel_num = [int(i) for i in str_pixel_num]  #타입변환
        else:
            #i = 명암값 / pixel_num 명암값 빈도 수 저장
            for i in intensity:
                pixel_num[i] += 1

            # 이미 csv파일 있는 경우
            with open(self.image_name + '.csv', 'w', encoding="utf-8") as fp:  # 명암 값 csv로 저장
                for i in range(255):
                    fp.write(str(pixel_num[i]) + ',')
                fp.write(str(pixel_num[255]))

            fp.close()

        self.intensity = intensity
        self.bright_hist = bright_hist
        self.pixel_num = pixel_num              #정보 저장 후 리턴

        return
