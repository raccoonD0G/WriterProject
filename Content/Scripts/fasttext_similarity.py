import fasttext
import sys
import re
import os
import numpy as np

# 현재 스크립트 파일이 위치한 디렉토리를 기준으로 상대 경로 설정
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))

KOREAN_MODEL_PATH = os.path.join(SCRIPT_DIR, "wiki.ko.bin")
ENGLISH_MODEL_PATH = os.path.join(SCRIPT_DIR, "wiki.en.bin")

# FastText 모델 로드 (한 번만 수행)
kor_model = fasttext.load_model(KOREAN_MODEL_PATH)
eng_model = fasttext.load_model(ENGLISH_MODEL_PATH)

def is_korean(word):
    """단어가 한글인지 확인하는 함수"""
    return bool(re.search("[가-힣]", word))

def get_similarity(word1, word2):
    """두 단어의 코사인 유사도 계산 (최적화된 버전)"""
    try:
        # 단어별 모델 선택 (한 번만 판별)
        model = kor_model if is_korean(word1) else eng_model
        
        # 단어 벡터 가져오기
        vec1 = model.get_word_vector(word1)
        vec2 = model.get_word_vector(word2)

        # NumPy를 활용한 벡터 연산 (속도 향상)
        dot_product = np.dot(vec1, vec2)
        norm1 = np.linalg.norm(vec1)
        norm2 = np.linalg.norm(vec2)

        # 유사도 계산
        if norm1 == 0 or norm2 == 0:
            return "Unknown Word"

        return dot_product / (norm1 * norm2)
    except Exception as e:
        return str(e)

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python fasttext_similarity.py <word1> <word2>")
        sys.exit(1)

    word1 = sys.argv[1]
    word2 = sys.argv[2]

    print(get_similarity(word1, word2))
