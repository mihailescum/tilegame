print("Hello World")

entity1 = _create_entity()
entity2 = _create_entity()
print(entity1, entity2)

c = _ScriptLoaderComponent.new("test")
_add_component(entity1, c)