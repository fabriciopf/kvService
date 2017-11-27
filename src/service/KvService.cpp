#include <aws/core/utils/Outcome.h>
#include <aws/dynamodb/model/AttributeDefinition.h>
#include <aws/dynamodb/model/PutItemRequest.h>
#include <aws/dynamodb/model/PutItemResult.h>
#include <aws/dynamodb/model/GetItemRequest.h>
#include <aws/kms/model/EncryptRequest.h>
#include <aws/kms/model/EncryptResult.h>
#include <aws/kms/model/DecryptRequest.h>
#include <aws/kms/model/DecryptResult.h>

#include "KvService.hpp"
#include "rest.h"

KvService::KvService() {
	Aws::InitAPI(options);
	Aws::Client::ClientConfiguration clientConfig;
	dynamoClient = std::make_shared<Aws::DynamoDB::DynamoDBClient>(clientConfig);
	kmsClient = std::make_shared<Aws::KMS::KMSClient>();
}

KvService::~KvService() {
	Aws::ShutdownAPI(options);
}

bool KvService::encrypt(Aws::String& plainText, Aws::Utils::ByteBuffer& cipherText) {
	Aws::KMS::Model::EncryptRequest kmsRequest;
	Aws::Utils::ByteBuffer buffer((unsigned char*) plainText.data(), plainText.length());
	kmsRequest.SetKeyId(keyId);
	kmsRequest.SetPlaintext(buffer);
	//kmsRequest.setEncryptionContext(Collections.singletonMap("key", key));
	Aws::KMS::Model::EncryptOutcome outcome = kmsClient.get()->Encrypt(kmsRequest);
	cipherText = outcome.GetResult().GetCiphertextBlob();
	return outcome.IsSuccess();

	//    final Map<String, AttributeValue> item = new HashMap<>();
	//    item.put(EMAIL, new AttributeValue().withS(email));
	//    item.put(ADDRESS, new AttributeValue().withS(address));
	//    final Map<String, AttributeValue> encryptedItem = cryptor.encryptAllFieldsExcept(item, ddbCtx, EMAIL);
}

bool KvService::decrypt(Aws::Utils::ByteBuffer& cipherText, Aws::Utils::ByteBuffer& plainText) {
	Aws::KMS::Model::DecryptRequest kmsRequest;
	//kmsRequest.setEncryptionContext(Collections.singletonMap("key", key));
	Aws::KMS::Model::DecryptOutcome outcome = kmsClient.get()->Decrypt(kmsRequest.WithCiphertextBlob(cipherText));
	plainText = outcome.GetResult().GetPlaintext();
	return outcome.IsSuccess();

	//    final Map<String, AttributeValue> encryptedItem = ddb.getItem(TABLE, Collections.singletonMap(EMAIL, new AttributeValue().withS(email))).getItem();
	//    final Map<String, AttributeValue> item = cryptor.decryptAllFieldsExcept(encryptedItem, ddbCtx, EMAIL);
}

pplx::task<bool> KvService::putItem(const kv::proto::set_request& request) {
	return pplx::create_task([request, this] {
		Aws::DynamoDB::Model::PutItemRequest pir;
		pir.SetTableName(tableName);
		pir.AddItem("key", Aws::DynamoDB::Model::AttributeValue(request.req().key().c_str()));
		Aws::Utils::ByteBuffer cipherText;
		Aws::String plainText(request.req().value().c_str());
		if (!encrypt(plainText, cipherText)) {
			return false;
		}
		Aws::DynamoDB::Model::AttributeValue value;
		value.SetB(cipherText);
		pir.AddItem("value", value);

		const Aws::DynamoDB::Model::PutItemOutcome result = dynamoClient.get()->PutItem(pir);
		if (!result.IsSuccess()) {
			std::cout << result.GetError().GetMessage() << std::endl;
			return false;
		}

		std::cout << "Item inserted!" << std::endl;
		return true;
	});
}

pplx::task<bool> KvService::getItem(const kv::proto::get_request& request, std::shared_ptr<kv::proto::data> data) {
	return pplx::create_task([request, this, data] {

		Aws::DynamoDB::Model::AttributeValue hashKey;
		hashKey.SetS(request.key().c_str());
		Aws::DynamoDB::Model::GetItemRequest req;
		req.AddKey("key", hashKey);
		req.SetTableName(tableName);
		const Aws::DynamoDB::Model::GetItemOutcome& result = dynamoClient.get()->GetItem(req);
		if (!result.IsSuccess()) {
			std::cout << "Failed to get item: " << result.GetError().GetMessage();
			return false;
		}

		const Aws::Map<Aws::String, Aws::DynamoDB::Model::AttributeValue>& item = result.GetResult().GetItem();
		if (!item.size()) {
			return false;
		}

		data.get()->set_key(item.find("key")->second.GetS().data());
		Aws::Utils::ByteBuffer cipherText = item.find("value")->second.GetB();
		Aws::Utils::ByteBuffer plainText;
		if (!decrypt(cipherText, plainText)) {
			return false;
		}

		std::string value = (char*) plainText.GetUnderlyingData();
		value.resize(plainText.GetLength());
		data.get()->set_value(value);

		return true;
	});
}


