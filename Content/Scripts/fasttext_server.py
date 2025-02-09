import fasttext
import os
import re
import numpy as np
from flask import Flask, request, jsonify

# 현재 스크립트 디렉토리 기준으로 모델 로드
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))

KOREAN_MODEL_PATH = os.path.join(SCRIPT_DIR, "wiki.ko.bin")
ENGLISH_MODEL_PATH = os.path.join(SCRIPT_DIR, "wiki.en.bin")

app = Flask(__name__)

# 모델을 전역 변수로 한 번만 로드
kor_model = None
eng_model = None

def load_models():
    global kor_model, eng_model
    if kor_model is None or eng_model is None:
        print("Loading FastText models... (This may take a while)")
        try:
            kor_model = fasttext.load_model(KOREAN_MODEL_PATH)
            eng_model = fasttext.load_model(ENGLISH_MODEL_PATH)
            print("Models loaded successfully!")
        except Exception as e:
            print(f"Error loading models: {e}")
            exit(1)

def is_korean(word):
    """단어가 한글인지 확인하는 함수"""
    return bool(re.search("[가-힣]", word))

@app.route("/similarity", methods=["POST"])
def get_similarity():
    """두 단어의 유사도를 계산하는 API"""
    try:
        load_models()  # 서버가 재시작되어도 중복 로드 방지

        # 요청에서 JSON 데이터 가져오기
        data = request.json
        if not data:
            print("Error: No JSON received")
            return jsonify({"error": "No JSON received"}), 400

        word1 = data.get("word1")
        word2 = data.get("word2")

        if not word1 or not word2:
            print(f"Error: Missing words. Received: {data}")
            return jsonify({"error": "Missing words"}), 400

        model = kor_model if is_korean(word1) else eng_model

        # FastText 벡터 가져오기 (예외 처리 추가)
        try:
            vec1 = model.get_word_vector(word1)
            vec2 = model.get_word_vector(word2)
        except Exception as e:
            print(f"FastText Error: {str(e)}")
            return jsonify({"similarity": -1.0, "error": f"FastText Error: {str(e)}"}), 200

        # 벡터 연산
        dot_product = np.dot(vec1, vec2)
        norm1 = np.linalg.norm(vec1)
        norm2 = np.linalg.norm(vec2)

        if norm1 == 0 or norm2 == 0:
            return jsonify({"similarity": -1.0, "error": "Zero vector"}), 200

        similarity = dot_product / (norm1 * norm2)

        # Python float 변환 (JSON 직렬화 오류 해결)
        similarity = float(similarity)

        return jsonify({"similarity": similarity})

    except Exception as e:
        print(f"Server Error: {e}")
        return jsonify({"error": f"Server Error: {str(e)}"}), 500

if __name__ == "__main__":
    load_models()  # 실행 시 모델을 한 번만 로드
    app.run(host="0.0.0.0", port=5000, debug=False)
