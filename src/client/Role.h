#ifndef CLIENT_ROLE_H_
#define CLIENT_ROLE_H_

class Role {
 public:
  Role() = default;
  Role(Role&) = delete;
  Role(Role&&) = delete;
  Role& operator=(Role const&) = delete;
  Role& operator=(Role&&) = delete;
  virtual void Start();
  virtual void LoginOrRegister();
  virtual void Login() = 0;
  virtual void Register() {}
  virtual void MainMenu() = 0;
  virtual ~Role(){};
};

#endif  // CLIENT_ROLE_H_