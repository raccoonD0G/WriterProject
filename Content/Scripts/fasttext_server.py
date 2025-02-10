import fasttext
import os
import re
import numpy as np
from flask import Flask, request, jsonify

# 현재 스크립트 디렉토리 기준으로 모델 경로 설정
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
KOREAN_MODEL_PATH = os.path.join(SCRIPT_DIR, "wiki.ko.bin")
ENGLISH_MODEL_PATH = os.path.join(SCRIPT_DIR, "wiki.en.bin")

app = Flask(__name__)

# 모델 전역 변수 (최초 요청 시 로드)
kor_model = None
eng_model = None

def load_models():
    global kor_model, eng_model
    if kor_model is None or eng_model is None:
        print("Loading FastText models... (This may take a while)")
        kor_model = fasttext.load_model(KOREAN_MODEL_PATH)
        eng_model = fasttext.load_model(ENGLISH_MODEL_PATH)
        print("Models loaded successfully!")

def is_korean(word):
    """단어에 한글이 포함되어 있는지 확인"""
    return bool(re.search("[가-힣]", word))

@app.route("/similarity", methods=["POST"])
def get_similarity():
    load_models()
    data = request.get_json()
    if not data or "word1" not in data or "word2" not in data:
        return jsonify({"error": "Missing words"}), 400

    word1, word2 = data["word1"], data["word2"]
    model = kor_model if is_korean(word1) else eng_model

    # FastText 벡터 추출 및 유사도 계산
    vec1 = model.get_word_vector(word1)
    vec2 = model.get_word_vector(word2)
    norm1, norm2 = np.linalg.norm(vec1), np.linalg.norm(vec2)

    if norm1 == 0 or norm2 == 0:
        return jsonify({"similarity": -1.0, "error": "Zero vector"}), 200

    similarity = float(np.dot(vec1, vec2) / (norm1 * norm2))
    return jsonify({"similarity": similarity})

if __name__ == "__main__":
    load_models()
    app.run(host="0.0.0.0", port=5001)
