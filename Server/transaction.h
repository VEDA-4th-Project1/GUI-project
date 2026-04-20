#pragma once

// 계좌의 단일 거래 내역을 나타내는 구조체.
// Account 내부의 m_transactions 벡터에 쌓이며,
// accounts.json에 직렬화되어 영구 저장된다.

#include <QString>
#include <QDateTime>
#include <QJsonObject>

struct Transaction {
    QString   type;        // 거래 종류: "deposit"(입금) 또는 "withdraw"(출금)
    double    amount;      // 거래 금액 (원)
    QDateTime timestamp;   // 거래 발생 시각
    QString   description; // 거래 설명 (예: "초기 입금", "월급")

    // Transaction 객체를 QJsonObject로 변환 (파일 저장 시 사용)
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["type"]        = type;
        obj["amount"]      = amount;
        obj["timestamp"]   = timestamp.toString(Qt::ISODate);
        obj["description"] = description;
        return obj;
    }

    // QJsonObject에서 Transaction 객체를 복원 (파일 로드 시 사용)
    static Transaction fromJson(const QJsonObject& obj) {
        return {
            obj["type"].toString(),
            obj["amount"].toDouble(),
            QDateTime::fromString(obj["timestamp"].toString(), Qt::ISODate),
            obj["description"].toString()
        };
    }
};
