import tensorflow as tf
import numpy as np

from tensorflow.keras.layers import Conv2D, Input, Dense, MaxPool2D, BatchNormalization, GlobalAvgPool2D 
from tensorflow.python.keras import activations

from deepLearningModels import functional_model,MyCustomModel
from my_utils import display_some_examples



if __name__=='__main__':

    (x_train,y_train),(x_test,y_test) = tf.keras.datasets.mnist.load_data()
    
    print("x_train shape = ", x_train.shape)

    if False:
        display_some_examples(x_train, y_train)

    x_train = x_train.astype('float32')/255
    x_test = x_test.astype('float32')/255

    x_train = np.expand_dims(x_train,axis=-1)
    x_test = np.expand_dims(x_test,axis=-1)

    # model = functional_model()
    #  model.summary()

    model = MyCustomModel()
  
    model.compile(optimizer='Adam', loss='sparse_categorical_crossentropy', metrics='Accuracy')
    model.fit(x_train,y_train, batch_size=64, epochs=3, validation_split=0.2)

    model.summary()
    model.evaluate(x_test,y_test,batch_size=64)

