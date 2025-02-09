import json
from flask import Flask, request, Response
from transformers import AutoTokenizer, AutoModelForTokenClassification, pipeline

app = Flask(__name__)

# mBERT 기반 NER 모델 불러오기 (Davlan 모델 예시)
tokenizer = AutoTokenizer.from_pretrained("Davlan/bert-base-multilingual-cased-ner-hrl")
model = AutoModelForTokenClassification.from_pretrained("Davlan/bert-base-multilingual-cased-ner-hrl")
# aggregation_strategy 옵션을 추가하여 엔티티 단위로 집계함
ner_pipeline = pipeline("ner", model=model, tokenizer=tokenizer, aggregation_strategy="simple")

@app.route('/extract_named_entities', methods=['POST'])
def extract_named_entities():
    data = request.json
    text = data.get("text", "")
    
    results = ner_pipeline(text)
    # mBERT NER 모델의 엔티티 그룹: PER, ORG, LOC, MISC
    proper_noun_tags = ["PER", "ORG", "LOC", "MISC"]
    named_entities = [
        result['word'] 
        for result in results 
        if result.get('entity_group') in proper_noun_tags
    ]
    
    json_response = json.dumps({"named_entities": named_entities}, ensure_ascii=False)
    return Response(json_response, mimetype='application/json')

if __name__ == '__main__':
    app.run(host="0.0.0.0", port=5000)
