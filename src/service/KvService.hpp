#ifndef SERVICE_KVSERVICE_HPP_
#define SERVICE_KVSERVICE_HPP_

#include <aws/core/Aws.h>
#include <aws/dynamodb/DynamoDBClient.h>
#include <aws/kms/KMSClient.h>

#include "IKvService.hpp"

class KvService final : public IKvService {
protected:
	KvService();
	std::shared_ptr<Aws::DynamoDB::DynamoDBClient> dynamoClient;
	std::shared_ptr<Aws::KMS::KMSClient> kmsClient;

	const Aws::SDKOptions options;
	const Aws::String tableName{"kvTable"};
	const Aws::String keyId{"7a1757f3-fb59-4867-a4d9-11ce281455da"};

	bool encrypt(Aws::String& plainText, Aws::Utils::ByteBuffer& cipherText);
	bool decrypt(Aws::Utils::ByteBuffer& cipherText, Aws::Utils::ByteBuffer& plainText);

public:
	~KvService();
	static KvService& getInstance() {static KvService instance; return instance;};
	pplx::task<bool> putItem(const kv::proto::set_request& request) override;
	pplx::task<bool> getItem(const kv::proto::get_request& request, std::shared_ptr<kv::proto::data> data) override;
};

#endif /* SERVICE_KVSERVICE_HPP_ */
