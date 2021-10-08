#pragma once
class HealthComponent final : public BaseComponent
{
public:

	HealthComponent(const int maxHealth);
	virtual ~HealthComponent() = default;

	HealthComponent(const HealthComponent& other) = delete;
	HealthComponent(HealthComponent&& other) noexcept = delete;
	HealthComponent& operator=(const HealthComponent& other) = delete;
	HealthComponent& operator=(HealthComponent&& other) noexcept = delete;

	int GetCurrHealth()const;
	int GetMaxHealth()const;
	void AddHealth(int amountOfHealth);
	void Damage(int amount);
protected:
	void Update(const GameContext& context) override;
	void Draw(const GameContext& context) override;
	void Initialize(const GameContext& gameContext) override;
private:
	int m_MaxHealth;
	int m_CurrentHealth = 0;

	void Die();
};
