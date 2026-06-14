# ReTTR

ReTTR全称Rewrite Technology Type Reflection（重写的技术类型反射），是对RTTR库的技术性重写库。 用于简化RTTR库的API的同时，进行现代化改造。基于C++17标准开发，为C++26提供新的预览支持。
部分代码基于rainy-toolkit的部分源代码进行迁移。使用RTTR作为reference，确保在开发中，始终拥有可参照的API。

## 与RTTR的关系

ReTTR并非RTTR的衍生作品，其发行许可证采用Apache 2.0，而RTTR使用MIT。并且，ReTTR本体库源代码，均来自上游rainy-toolkit的代码。RTTR在此项目仅用于作为实现参考部分，进行重写。

另外，以及，在大部分接口中，ReTTR继承了RTTR的思想，但是，也同时删除了RTTR库中较为古老的API体系：

- policy （由明确的shared_object、object_view、object取代）
- variant_sequential_view, variant_associative_view, variant... （由any取代）
- sequential_container_mapper, associative_container_mapper（由any提供更明确的约束）
- instance （由object_view取代）
- wrapper 机制（可能导致意外的情况和不合理的预期之外结果，应当被shared_object、weak_object、object取代，避免自定义包装类）
- visitor（无专门替代实现的必要，可能导致更加难以预测或是导致维护性降低）

根据情况再进行删减

## 另注

ReTTR的代码依赖了rainy-toolkit项目中的源代码，按照Apache 2.0许可证分发。